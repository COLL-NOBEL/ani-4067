# VR/AR Latency Experiment: Results and Analysis

## 1. Raw Data Summary

The following data was collected across four separate trials of 1,000 frames each, tracking rendering latency and frame duration spikes.

| Metric | Trial 1 | Trial 2 | Trial 3 | Trial 4 | 
| ----- | ----- | ----- | ----- | ----- | 
| **Average Frame Rate (FPS)** | 27.7 | 36.7 | 35.3 | 36.1 | 
| **Longest Frame Duration (ms)** | 9130.92 | 7691.24 | 8349.74 | 8403.07 | 
| **Total Frames > 11ms** | 997 | 1000 | 1000 | 999 | 
| ├─ *Frames > 100ms* | 5 | 2 | 3 | 2 | 
| ├─ *Frames 50ms - 99.99ms* | 4 | 2 | 0 | 1 | 
| ├─ *Frames 20ms - 49.99ms* | 195 | 6 | 34 | 2 | 
| └─ *Frames 11ms - 19.99ms* | 793 | 990 | 963 | 994 | 

## 2. Calculated Averages (Across 4 Trials)

To establish a baseline of performance, the metrics from all four trials have been averaged below:

* **Average Frame Rate:** 33.95 FPS

* **Average Longest Frame:** 8393.74 ms

* **Average Total Frames > 11ms:** 999 frames (Out of 1,000)

  * **> 100ms:** 3 frames

  * **50ms - 99.99ms:** 1.75 frames

  * **20ms - 49.99ms:** 59.25 frames

  * **11ms - 19.99ms:** 935 frames

## 3. Implications for VR and AR

Based on the calculated averages, this application's current performance profile is entirely unsuitable for Virtual Reality or Augmented Reality deployment. Here is a brief breakdown of the implications:

### A. Severe Motion Sickness (Average 33.95 FPS)

The industry standard for VR is a minimum of 90 FPS (or 72 FPS for some standalone headsets like the Meta Quest). Running at an average of \~34 FPS means the physical movement of the user's head will severely desynchronize with the virtual environment. This latency causes visual lag (the world "swims" or trails behind the user's movement), which rapidly induces nausea and disorientation.

### B. Universal Target Failure (999 / 1000 frames failed)

To maintain 90 FPS, a frame must render in under 11.11 milliseconds. Your data shows that out of 1,000 frames, an average of 999 frames took longer than 11ms. This indicates that the base rendering cost of your application is fundamentally too heavy for real-time VR/AR.

### C. Application Hangs (Longest Frame: \~8.4 seconds)

The most alarming metric is the longest frame duration, averaging \~8,393 milliseconds (over 8 seconds). In a VR/AR environment:

* A frame taking over 100ms causes a heavy stutter.

* A frame taking 8 seconds is a complete application freeze.

* During an 8-second hang, the VR headset compositor will likely kick the user out of the application and into the system's safe loading environment to protect them from motion sickness.

* *Note:* A spike this massive often occurs right at the start of an application (due to asset loading, shader compilation, or heavy initialization scripts). It is highly recommended to let the application run for a few seconds *before* starting the 1,000-frame measurement to see if this is a startup issue or an ongoing runtime issue.

### Summary

To be viable for VR/AR, the average frame rate needs to triple, and the number of frames exceeding the 11ms threshold needs to drop from 999 to as close to 0 as possible.

## 4. Experimental Code (Unity C#)

The following C# script was used in Unity to conduct this experiment. It tracks delta time over 1,000 frames and categorizes latency spikes.

```csharp
using UnityEngine;

public class LatencyTest : MonoBehaviour
{
    private int frameCount = 0;
    private float maxFrameTime = 0f;
    private float totalTimeAccumulated = 0f;

    // Tracking variables
    private int framesOver11msTotal = 0;
    private int framesOver100ms = 0;
    private int frames50to99ms = 0;
    private int frames20to49ms = 0;
    private int frames11to19ms = 0;

    void Update()
    {
        // Stop executing after 1000 frames
        if (frameCount >= 1000) return;

        // Measure time in both seconds (for FPS) and milliseconds (for latency)
        float deltaTime = Time.unscaledDeltaTime;
        float frameDurationMs = deltaTime * 1000f;
        
        totalTimeAccumulated += deltaTime;

        // 1. Record the duration of the longest frame
        if (frameDurationMs > maxFrameTime)
        {
            maxFrameTime = frameDurationMs;
        }

        // 2. Filter and categorize latency spikes
        if (frameDurationMs > 11f)
        {
            framesOver11msTotal++;

            if (frameDurationMs >= 100f)
            {
                framesOver100ms++;
            }
            else if (frameDurationMs >= 50f)
            {
                frames50to99ms++;
            }
            else if (frameDurationMs >= 20f)
            {
                frames20to49ms++;
            }
            else
            {
                frames11to19ms++;
            }
        }

        frameCount++;

        // 3. Output results exactly once when the 1000th frame finishes
        if (frameCount == 1000)
        {
            float averageFps = 1000f / totalTimeAccumulated;
            
            Debug.Log("Experiment Complete (1000 frames).");
            Debug.Log($"Average Frame Rate: {averageFps:F1} FPS");
            Debug.Log($"Longest Frame Duration: {maxFrameTime:F2} ms");
            Debug.Log($"Total Frames > 11ms: {framesOver11msTotal}");
            Debug.Log($"  ├─ Frames > 100ms: {framesOver100ms}");
            Debug.Log($"  ├─ Frames 50ms - 99.99ms: {frames50to99ms}");
            Debug.Log($"  ├─ Frames 20ms - 49.99ms: {frames20to49ms}");
            Debug.Log($"  └─ Frames 11ms - 19.99ms: {frames11to19ms}");
        }
    }
}
```