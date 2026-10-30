# VR Motion-to-Photon Latency Pipeline & Constructor Documentation

In Virtual Reality (VR) engine architecture, maintaining a low and consistent **Motion-to-Photon (M2P) latency** (ideally under $20\text{ ms}$) is essential to prevent motion sickness and ensure spatial presence. 

Below is the structured breakdown of the 5 core stages of the VR pipeline, including typical time costs, official hardware constructor specifications, and verified testing methods.

---

## 1. Summary Table: VR Latency Pipeline

| Phase | Time Cost | Official Constructor Source / Specification | Primary Testing & Profiling Tool |
| :--- | :--- | :--- | :--- |
| **1. Sensors measure movement** | **1 to 2 ms** | **Khronos OpenXR / Meta IMU Specs**<br>IMU sampling rates ($1000\text{ Hz}$) and sensor fusion algorithms. | OpenXR Space Locators (`xrLocateSpace`) / Optical Tracking Rigs |
| **2. System transmits measurement** | **1 to 3 ms** | **Meta Horizon OS / Oculus Driver Architecture**<br>USB/Bluetooth polling, driver interrupts, and IPC transmission. | Meta Quest Developer Hub (MQDH) System Metrics / Driver Profilers |
| **3. Application decides & draws** | **5 to 11 ms** | **Meta Horizon OS Performance Guides**<br>Game thread logic, frame scheduling, and GPU rendering budget. | Meta OVR Metrics Tool / Unreal Insights / Unity Profiler |
| **4. Compositor assembles** | **1 to 2 ms** | **Meta Horizon OS: Asynchronous TimeWarp (ATW)**<br>Layer composition, late orientation warp, and distortion correction. | Meta Performance Overlay / OpenXR Validation Layers |
| **5. Screen displays the line** | **2 to 5 ms** | **Oculus & Valve Display Specifications**<br>Pixel response time, low persistence, and row-by-row display scan-out. | Hardware Photodiode Test Rigs (e.g., OptoFidelity Buddy) |

---

## 2. Detailed Breakdown & Official References

### Phase 1: Sensors Measure Movement ($1 \text{ to } 2\text{ ms}$)
* **Description:** The Inertial Measurement Unit (IMU)—consisting of gyroscopes and accelerometers—samples raw physical motion data and passes it through sensor fusion filters.
* **Official Constructor Documentation:**
  * [Khronos OpenXR 1.0 Specification (`xrLocateSpace`)](https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html)
  * [PMC VR Sensor Motion-to-Photon Measurement Study](https://pmc.ncbi.nlm.nih.gov/articles/PMC10616216/)
* **Testing & Verification:**
  * Modern VR IMUs sample at **$1000\text{ Hz}$** ($1\text{ ms}$ per sample).
  * Tested by querying high-frequency pose prediction through OpenXR or using synchronized high-speed optical capture.

---

### Phase 2: System Transmits the Measurement ($1 \text{ to } 3\text{ ms}$)
* **Description:** The time required for raw sensor data to travel through hardware buses (USB/SPI/Bluetooth), trigger driver interrupts, and reach the VR runtime process.
* **Official Constructor Documentation:**
  * [Meta OpenXR Frame Pacing & Settings Guide](https://developers.meta.com/vr/documentation/unity/unity-openxr-settings/)
  * [John Carmack: Oculus Latency Mitigation Strategies](https://danluu.com/latency-mitigation/)
* **Testing & Verification:**
  * Measured via kernel-level event tracing (ETW on Windows / `ftrace` on Android/Meta Horizon OS) to track packet arrival times from the USB/HID driver to the runtime space.

---

### Phase 3: Application Decides and Draws ($5 \text{ to } 11\text{ ms}$)
* **Description:** The CPU updates game logic, physics, and scene graphs, submitting draw calls to the GPU, which then renders the stereo images into swapchain textures.
* **Official Constructor Documentation:**
  * [Meta Horizon OS Developer Resources: Performance & Optimization](https://developers.meta.com/vr/resources/publish-overview-performance/)
  * [Meta Unity Performance & Profiling Guide](https://developers.meta.com/vr/documentation/unity/unity-perf/)
* **Testing & Verification:**
  * To hit **$90\text{ Hz}$**, total frame budget is **$11.1\text{ ms}$**. To hit **$120\text{ Hz}$**, the budget drops to **$8.33\text{ ms}$**.
  * Tested on-device using the **Meta OVR Metrics Tool** overlay or engine-level CPU/GPU markers (`Unreal Insights` / `Unity Profiler`).

---

### Phase 4: Compositor Assembles ($1 \text{ to } 2\text{ ms}$)
* **Description:** The system compositor interrupts right before vsync, samples the latest IMU pose, and performs **Asynchronous TimeWarp (ATW)** to warp the final image and correct lens distortion.
* **Official Constructor Documentation:**
  * [Meta Horizon OS Native Docs: Compositor & TimeWarp API](https://developers.meta.com/vr/documentation/native/pc/dg-compositor-timewarp/)
  * [Meta Horizon Engineering Blog: The Latent Power of Prediction](https://developers.meta.com/vr/blog/the-latent-power-of-prediction/)
* **Testing & Verification:**
  * Verified using compositor timing overlays in the Meta Quest Developer Hub (MQDH), which log precise hardware compositor slice pass duration ($1\text{–}2\text{ ms}$).

---

### Phase 5: Screen Displays the Line ($2 \text{ to } 5\text{ ms}$)
* **Description:** The display controller scans out pixels line-by-line onto the OLED or Fast-Switch LCD panel, using low-persistence illumination to prevent motion blur.
* **Official Constructor Documentation:**
  * [OptoFidelity Buddy Hardware Latency Tester](https://www.optofidelity.com/products/optofidelity-buddy)
  * [Michael Abrash / Valve Research: Display Scanout and Persistence](https://xinreality.com/wiki/Motion-to-photon_latency)
* **Testing & Verification:**
  * Requires physical hardware testing using high-speed photodiodes ($10,000\text{ Hz}$) affixed to the VR display lens to measure the exact delta between controller motion and pixel luminance change.