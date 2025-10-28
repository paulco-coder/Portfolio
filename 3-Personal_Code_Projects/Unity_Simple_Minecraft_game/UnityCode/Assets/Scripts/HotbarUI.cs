using UnityEngine;
using UnityEngine.UI;

public class HotbarUI : MonoBehaviour
{
    [System.Serializable]
    private class Slot
    {
        public BlockInteractor.BlockType blockType;
        public Image frame;
        public RawImage icon;
    }

    [Header("References")]
    [SerializeField] private BlockInteractor blockInteractor;
    [SerializeField] private Slot[] slots;
    [SerializeField] private Text selectedLabel;

    [Header("Colors")]
    [SerializeField] private Color normalColor = new Color(0.2f, 0.2f, 0.2f, 0.9f);
    [SerializeField] private Color selectedColor = new Color(0.9f, 0.9f, 0.3f, 1f);

    private void Awake()
    {
        if (blockInteractor == null)
        {
            blockInteractor = FindObjectOfType<BlockInteractor>();
        }
    }

    private void OnEnable()
    {
        if (blockInteractor == null)
        {
            return;
        }

        blockInteractor.SelectionChanged += Refresh;
        UpdateIcons();
        Refresh(blockInteractor.CurrentSelection);
    }

    private void OnDisable()
    {
        if (blockInteractor != null)
        {
            blockInteractor.SelectionChanged -= Refresh;
        }
    }

    private void UpdateIcons()
    {
        if (blockInteractor == null)
        {
            return;
        }

        foreach (Slot slot in slots)
        {
            if (slot.icon == null)
            {
                continue;
            }

            Texture2D texture = blockInteractor.GetTextureFor(slot.blockType);
            if (texture != null)
            {
                slot.icon.texture = texture;
            }
        }
    }

    private void Refresh(BlockInteractor.BlockType active)
    {
        foreach (Slot slot in slots)
        {
            if (slot.frame != null)
            {
                slot.frame.color = slot.blockType == active ? selectedColor : normalColor;
            }
        }

        if (selectedLabel != null)
        {
            selectedLabel.text = $"Selected: {active}";
        }
    }
}
