using UnityEngine;

[RequireComponent(typeof(CharacterController))]
public class PlayerController : MonoBehaviour
{
    [Header("Movement")]
    [SerializeField] private float walkSpeed = 7.5f;
    [SerializeField] private float sprintMultiplier = 1.5f;
    [SerializeField] private float gravity = -20f;
    [SerializeField] private float jumpHeight = 1.5f;

    [Header("Double Tap Run")]
    [SerializeField] private float doubleTapWindow = 0.3f;
    [SerializeField] private float doubleTapSpeedMultiplier = 2f;

    [Header("Mouse Look")]
    [SerializeField] private float mouseSensitivity = 120f;
    [SerializeField] private Transform cameraPivot;

    private CharacterController controller;
    private float verticalVelocity;
    private float pitch;
    private float lastForwardTapTime = -10f;
    private bool doubleTapActive;

    private void Awake()
    {
        controller = GetComponent<CharacterController>();
    }

    private void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }

    private void Update()
    {
        Look();
        Move();
    }

    private void Look()
    {
        Vector2 mouse = new Vector2(Input.GetAxis("Mouse X"), Input.GetAxis("Mouse Y")) * mouseSensitivity * Time.deltaTime;

        transform.Rotate(Vector3.up * mouse.x);

        pitch -= mouse.y;
        pitch = Mathf.Clamp(pitch, -85f, 85f); // keep camera within a comfortable vertical range
        cameraPivot.localRotation = Quaternion.Euler(pitch, 0f, 0f);
    }

    private void Move()
    {
        if (controller.isGrounded && verticalVelocity < 0f)
        {
            verticalVelocity = -2f;
        }

        bool sprinting = Input.GetKey(KeyCode.LeftShift);
        bool forwardHeld = Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.Z) || Input.GetKey(KeyCode.UpArrow);
        bool forwardPressed = Input.GetKeyDown(KeyCode.W) || Input.GetKeyDown(KeyCode.Z) || Input.GetKeyDown(KeyCode.UpArrow);

        if (forwardPressed)
        {
            if (Time.time - lastForwardTapTime <= doubleTapWindow)
            {
                doubleTapActive = true;
            }

            lastForwardTapTime = Time.time;
        }

        if (!forwardHeld)
        {
            doubleTapActive = false;
        }

        float horizontal = Input.GetAxisRaw("Horizontal");
        float vertical = Input.GetAxisRaw("Vertical");

        Vector3 input = new Vector3(horizontal, 0f, vertical).normalized;
        Vector3 velocity = (transform.right * input.x + transform.forward * input.z) * walkSpeed;

        if (sprinting)
        {
            velocity *= sprintMultiplier;
        }

        if (doubleTapActive && forwardHeld && vertical > 0f)
        {
            velocity *= doubleTapSpeedMultiplier;
        }

        if (Input.GetButtonDown("Jump") && controller.isGrounded)
        {
            verticalVelocity = Mathf.Sqrt(jumpHeight * -2f * gravity);
        }

        verticalVelocity += gravity * Time.deltaTime;
        velocity.y = verticalVelocity;

        controller.Move(velocity * Time.deltaTime);
    }
}
