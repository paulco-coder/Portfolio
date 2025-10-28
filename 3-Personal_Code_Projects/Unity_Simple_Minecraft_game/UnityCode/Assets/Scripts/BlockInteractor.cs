using System;
using UnityEngine;

public class BlockInteractor : MonoBehaviour
{
    public enum BlockType
    {
        Grass,
        Dirt,
        Wood,
        Leaves
    }

    [Header("References")]
    [SerializeField] private Transform cameraTransform;
    [SerializeField] private Transform blockParent;

    [Header("Interaction")]
    [SerializeField] private float reachDistance = 6f;
    [SerializeField] private LayerMask blockMask = ~0;
    [SerializeField]
    private BlockType[] hotbarOrder =
    {
        BlockType.Grass,
        BlockType.Dirt,
        BlockType.Wood,
        BlockType.Leaves
    };

    [Header("Block Textures")]
    [SerializeField] private Texture2D grassTexture;
    [SerializeField] private Texture2D dirtTexture;
    [SerializeField] private Texture2D woodTexture;
    [SerializeField] private Texture2D leavesTexture;

    [SerializeField] private BlockType selectedBlock = BlockType.Grass;
    private Material grassMaterial;
    private Material dirtMaterial;
    private Material woodMaterial;
    private Material leavesMaterial;
    private int currentSelectionIndex;

    public BlockType CurrentSelection => selectedBlock;
    public event Action<BlockType> SelectionChanged;

    private void Awake()
    {
        CacheMaterials();
        if (cameraTransform == null && Camera.main != null)
        {
            cameraTransform = Camera.main.transform;
        }
        SyncSelectionIndex();
        NotifySelectionChanged();
    }

    private void Update()
    {
        HandleHotbarInput();

        if (Input.GetMouseButtonDown(0))
        {
            BreakBlock();
        }
        else if (Input.GetMouseButtonDown(1))
        {
            PlaceBlock();
        }
    }

    private void HandleHotbarInput()
    {
        if (Input.GetKeyDown(KeyCode.Alpha1))
        {
            SetSelectedBlock(BlockType.Grass);
        }
        else if (Input.GetKeyDown(KeyCode.Alpha2))
        {
            SetSelectedBlock(BlockType.Dirt);
        }
        else if (Input.GetKeyDown(KeyCode.Alpha3))
        {
            SetSelectedBlock(BlockType.Wood);
        }
        else if (Input.GetKeyDown(KeyCode.Alpha4))
        {
            SetSelectedBlock(BlockType.Leaves);
        }

        float scroll = Input.mouseScrollDelta.y;
        if (Mathf.Abs(scroll) > Mathf.Epsilon)
        {
            int direction = scroll > 0f ? -1 : 1; // scroll up usually selects previous slot
            CycleSelection(direction);
        }
    }

    private void SetSelectedBlock(BlockType type)
    {
        if (selectedBlock == type)
        {
            return;
        }

        selectedBlock = type;
        SyncSelectionIndex();
        NotifySelectionChanged();
    }

    private void BreakBlock()
    {
        if (!TryRaycast(out RaycastHit hit))
        {
            return;
        }

        Destroy(hit.collider.gameObject);
    }

    private void PlaceBlock()
    {
        if (!TryRaycast(out RaycastHit hit))
        {
            return;
        }

        Vector3 targetWorldPosition = hit.collider.transform.position + hit.normal;
        Vector3Int gridPosition;

        if (blockParent != null)
        {
            Vector3 local = blockParent.InverseTransformPoint(targetWorldPosition);
            gridPosition = Vector3Int.RoundToInt(local);
            targetWorldPosition = blockParent.TransformPoint(gridPosition);
        }
        else
        {
            gridPosition = Vector3Int.RoundToInt(targetWorldPosition);
            targetWorldPosition = gridPosition;
        }

        if (Physics.CheckBox(targetWorldPosition, Vector3.one * 0.45f, Quaternion.identity, blockMask, QueryTriggerInteraction.Ignore))
        {
            return; // spot already filled
        }

        SpawnBlock(gridPosition, ResolveMaterial(selectedBlock));
    }

    private bool TryRaycast(out RaycastHit hit)
    {
        hit = default;
        if (cameraTransform == null)
        {
            return false;
        }

        Ray ray = new Ray(cameraTransform.position, cameraTransform.forward);
        RaycastHit[] hits = Physics.RaycastAll(ray, reachDistance, blockMask, QueryTriggerInteraction.Ignore);

        if (hits.Length == 0)
        {
            return false;
        }

        Array.Sort(hits, (a, b) => a.distance.CompareTo(b.distance));

        foreach (RaycastHit candidate in hits)
        {
            if (candidate.collider.attachedRigidbody != null)
            {
                continue;
            }

            if (candidate.collider.GetComponent<CharacterController>() != null)
            {
                continue; // prevents deleting the local player controller capsule
            }

            if (blockParent != null && !candidate.collider.transform.IsChildOf(blockParent))
            {
                continue;
            }

            hit = candidate;
            return true;
        }

        return false;
    }

    private void SpawnBlock(Vector3Int localPosition, Material overrideMaterial)
    {
        GameObject block = GameObject.CreatePrimitive(PrimitiveType.Cube);
        if (blockParent != null)
        {
            block.transform.SetParent(blockParent, false);
            block.transform.localPosition = localPosition;
        }
        else
        {
            block.transform.position = localPosition;
        }

        MeshRenderer renderer = block.GetComponent<MeshRenderer>();
        if (renderer != null && overrideMaterial != null)
        {
            renderer.sharedMaterial = overrideMaterial;
        }
    }

    private Material ResolveMaterial(BlockType type)
    {
        return type switch
        {
            BlockType.Grass => grassMaterial ?? dirtMaterial,
            BlockType.Dirt => dirtMaterial ?? grassMaterial,
            BlockType.Wood => woodMaterial ?? grassMaterial,
            BlockType.Leaves => leavesMaterial ?? grassMaterial,
            _ => grassMaterial
        };
    }

    public Texture2D GetTextureFor(BlockType type)
    {
        return type switch
        {
            BlockType.Grass => grassTexture,
            BlockType.Dirt => dirtTexture,
            BlockType.Wood => woodTexture,
            BlockType.Leaves => leavesTexture,
            _ => null
        };
    }

    private void CacheMaterials()
    {
        grassMaterial = CreateMaterial(grassTexture);
        dirtMaterial = CreateMaterial(dirtTexture);
        woodMaterial = CreateMaterial(woodTexture);
        leavesMaterial = CreateMaterial(leavesTexture);
    }

    private Material CreateMaterial(Texture2D texture)
    {
        if (texture == null)
        {
            return null;
        }

        Shader shader = Shader.Find("Standard");
        Material mat = new Material(shader)
        {
            mainTexture = texture
        };
        mat.SetFloat("_Glossiness", 0f);
        return mat;
    }

    private void NotifySelectionChanged()
    {
        SelectionChanged?.Invoke(selectedBlock);
    }

    private void CycleSelection(int step)
    {
        if (hotbarOrder == null || hotbarOrder.Length == 0)
        {
            return;
        }

        currentSelectionIndex = (currentSelectionIndex + step) % hotbarOrder.Length;
        if (currentSelectionIndex < 0)
        {
            currentSelectionIndex += hotbarOrder.Length;
        }

        SetSelectedBlock(hotbarOrder[currentSelectionIndex]);
    }

    private void SyncSelectionIndex()
    {
        if (hotbarOrder == null || hotbarOrder.Length == 0)
        {
            hotbarOrder = new[] { selectedBlock };
        }

        int index = System.Array.IndexOf(hotbarOrder, selectedBlock);
        if (index < 0)
        {
            currentSelectionIndex = 0;
            selectedBlock = hotbarOrder[currentSelectionIndex];
        }
        else
        {
            currentSelectionIndex = index;
        }
    }
}
