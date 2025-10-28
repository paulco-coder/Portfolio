using UnityEngine;

public class TerrainGenerator : MonoBehaviour
{
    [Header("Chunk Size")]
    [SerializeField] private int chunkWidth = 16;
    [SerializeField] private int chunkLength = 16;
    [SerializeField] private int maxHeight = 8;

    [Header("Noise Settings")]
    [SerializeField] private float noiseScale = 0.1f;
    [SerializeField] private int seed = 12345;
    [SerializeField] private Vector2 noiseOffset = Vector2.zero;

    [Header("Trees")]
    [SerializeField, Range(0f, 1f)] private float treeChance = 0.15f;
    [SerializeField] private int minTreeHeight = 4;
    [SerializeField] private Vector2Int treeTrunkHeightRange = new Vector2Int(3, 5);
    [SerializeField] private bool generateTrees = true;

    [Header("Block Textures")]
    [SerializeField] private Texture2D grassTexture;
    [SerializeField] private Texture2D dirtTexture;
    [SerializeField] private Texture2D woodTexture;
    [SerializeField] private Texture2D leavesTexture;

    private Material grassMaterial;
    private Material dirtMaterial;
    private Material woodMaterial;
    private Material leavesMaterial;

    private void Start()
    {
        CacheMaterials();
        ClearExistingBlocks();
        GenerateChunk();
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

    private void ClearExistingBlocks()
    {
        for (int i = transform.childCount - 1; i >= 0; i--)
        {
            Transform child = transform.GetChild(i);
            if (Application.isPlaying)
            {
                Destroy(child.gameObject);
            }
            else
            {
                DestroyImmediate(child.gameObject);
            }
        }
    }

    private void GenerateChunk()
    {
        Random.InitState(seed);

        for (int x = 0; x < chunkWidth; x++)
        {
            for (int z = 0; z < chunkLength; z++)
            {
                float sampleX = (x + noiseOffset.x) * noiseScale;
                float sampleZ = (z + noiseOffset.y) * noiseScale;
                float noiseValue = Mathf.PerlinNoise(sampleX, sampleZ);
                int height = Mathf.Clamp(Mathf.RoundToInt(noiseValue * maxHeight), 1, maxHeight);

                for (int y = 0; y < height; y++)
                {
                    Material targetMaterial = SelectMaterialForHeight(y, height);
                    SpawnBlock(new Vector3Int(x, y, z), targetMaterial);
                }

                if (generateTrees && height >= minTreeHeight && Random.value <= treeChance)
                {
                    TrySpawnTree(new Vector3Int(x, height, z));
                }
            }
        }
    }

    private Material SelectMaterialForHeight(int currentY, int columnHeight)
    {
        if (currentY == columnHeight - 1)
        {
            return grassMaterial ?? dirtMaterial;
        }

        if (currentY > 0)
        {
            return dirtMaterial ?? grassMaterial;
        }

        return woodMaterial ?? dirtMaterial ?? grassMaterial;
    }

    private void SpawnBlock(Vector3Int localPosition, Material overrideMaterial)
    {
        GameObject block = GameObject.CreatePrimitive(PrimitiveType.Cube);
        block.transform.SetParent(transform, false);
        block.transform.localPosition = localPosition;

        if (overrideMaterial != null)
        {
            block.GetComponent<MeshRenderer>().sharedMaterial = overrideMaterial;
        }
    }

    private void TrySpawnTree(Vector3Int basePosition)
    {
        int trunkHeight = Mathf.Clamp(Random.Range(treeTrunkHeightRange.x, treeTrunkHeightRange.y + 1), 1, 8);

        // simple guard to avoid obvious overlaps near chunk borders
        if (basePosition.x < 1 || basePosition.x > chunkWidth - 2 || basePosition.z < 1 || basePosition.z > chunkLength - 2)
        {
            return;
        }

        Vector3Int current = basePosition;
        for (int i = 0; i < trunkHeight; i++)
        {
            SpawnBlock(current, woodMaterial ?? dirtMaterial);
            current.y += 1;
        }

        SpawnLeavesCluster(current - Vector3Int.up);
    }

    private void SpawnLeavesCluster(Vector3Int topTrunkPosition)
    {
        Material targetMat = leavesMaterial ?? grassMaterial ?? dirtMaterial;
        const int radius = 2;

        for (int x = -radius; x <= radius; x++)
        {
            for (int y = 1; y <= radius; y++)
            {
                for (int z = -radius; z <= radius; z++)
                {
                    Vector3Int offset = new Vector3Int(x, y, z);
                    Vector3Int spawnPos = topTrunkPosition + offset;

                    if (Mathf.Abs(x) + Mathf.Abs(z) <= radius - (y - 1))
                    {
                        SpawnBlock(spawnPos, targetMat);
                    }
                }
            }
        }
    }
}
