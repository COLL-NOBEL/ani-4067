# VR Render Budget Estimation and Analysis

## 1. Raw Data Summary and Exclusions

Four trials were conducted to measure the isolated rendering time (excluding game logic) over $1000$ frames. 

*   **Trial 1:** $53.218 \text{ ms}$ (Single Eye) / $106.435 \text{ ms}$ (VR Render)
*   **Trial 2:** $27.769 \text{ ms}$ (Single Eye) / $55.539 \text{ ms}$ (VR Render)
*   **Trial 3:** $26.660 \text{ ms}$ (Single Eye) / $53.320 \text{ ms}$ (VR Render)
*   **Trial 4:** $26.086 \text{ ms}$ (Single Eye) / $52.173 \text{ ms}$ (VR Render)

**Note on Trial 1:** The first trial shows render times that are precisely double the subsequent trials. This is a common anomaly in game engines caused by initial background loading, shader compilation, or the engine "waking up." As requested, Trial 1 has been excluded from the averages below to ensure data accuracy.

## 2. The Measurement (Average of Trials 2-4)

By averaging the stable data from the last three trials, we establish the baseline measurement for a single camera rendering the scene:

*   **Average Single Eye Render Time:** $26.838 \text{ ms}$

## 3. The Estimation

In a traditional VR setup, the scene must be rendered twice (once per eye). We estimate this cost by multiplying the single-eye render time by two. We then compare this to our strict VR frame budget.

*   **Target VR Frame Budget ($90 \text{ FPS}$):** $11.110 \text{ ms}$
*   **Estimated VR Render Time ($\times 2$):** $53.677 \text{ ms}$
*   **Remaining Budget for Game Logic:** $0 \text{ ms}$

**Estimation Result:** Rendering alone consumes nearly five times the maximum allowed budget for a single frame. It exceeds the target by an average of $42.567 \text{ ms}$. Because the rendering phase takes so long, the engine physically cannot process physics, input, or scripts quickly enough to maintain a playable framerate. 

## 4. Conclusion: What Should Be Reduced?

Based on these measurements, the application is fundamentally unplayable in VR. Since this experiment was conducted on a program that "only clears the screen" (an empty or near-empty scene), a base render time of $\approx 27 \text{ ms}$ per eye is catastrophically high. 

To make this viable, you must heavily reduce the **base rendering overhead**. Here is what needs to be reduced or disabled immediately:

1.  **Reduce Unity Editor Overhead:** If you are running this test inside the Unity Editor, stop. The Editor itself consumes massive amounts of rendering resources. You must compile the project into a standalone executable (Build) to get accurate render latency measurements.
2.  **Disable VSync (Vertical Sync):** VSync forces the engine to wait for your computer monitor's refresh rate. If your monitor is $30 \text{ Hz}$ or $60 \text{ Hz}$, it artificially caps the render loop, inflating the `WaitForEndOfFrame` measurement. This must be turned off in Quality Settings.
3.  **Reduce Post-Processing:** Anti-aliasing (MSAA), Bloom, Ambient Occlusion, and Color Grading are extremely expensive. Ensure all post-processing volumes are deleted or disabled.
4.  **Reduce Resolution/Pixel Count:** Check the render scale. Rendering a blank screen at $4\text{K}$ resolution still takes time. Ensure the render resolution matches the target headset natively.
5.  **Change the Render Pipeline:** If you are using Unity's High Definition Render Pipeline (HDRP), the base overhead is too heavy for standard VR. You should reduce your pipeline complexity by switching to the Universal Render Pipeline (URP) or the Built-in Pipeline, and enable **Single Pass Instanced Rendering** (which reduces the VR rendering multiplier from $\times 2.0$ down to roughly $\times 1.2$).

## 5. Experimental Code (Unity C#)

Below is the C# script used to measure the render times and calculate the logic budget:

```csharp
using UnityEngine;
using System.Collections;
using System.Diagnostics; // Required for high-precision Stopwatch

public class VRRenderBudgetTest : MonoBehaviour
{
    private int frameCount = 0;
    private double totalRenderTimeMs = 0;
    
    // High-precision stopwatch for sub-millisecond accuracy
    private Stopwatch renderStopwatch = new Stopwatch();

    void Start()
    {
        // Start the coroutine to detect the exact end of the rendering phase
        StartCoroutine(MeasureRenderTime());
    }

    void LateUpdate()
    {
        // LateUpdate fires after all Update() logic is done, right before rendering begins.
        // We start/restart the stopwatch here to measure ONLY the rendering phase.
        if (frameCount < 1000)
        {
            renderStopwatch.Restart();
        }
    }

    IEnumerator MeasureRenderTime()
    {
        while (frameCount < 1000)
        {
            // Yields execution until the engine has finished rendering the frame
            yield return new WaitForEndOfFrame();
            
            renderStopwatch.Stop();
            totalRenderTimeMs += renderStopwatch.Elapsed.TotalMilliseconds;
            
            frameCount++;

            if (frameCount == 1000)
            {
                OutputEstimations();
                break; // Stop testing after 1000 frames
            }
        }
    }

    void OutputEstimations()
    {
        // 1. Measure the time rendering takes alone
        double averageRenderTime = totalRenderTimeMs / 1000.0;
        
        // 2. Estimate the cost of rendering twice (Traditional VR Multi-Pass rendering)
        double stereoRenderTime = averageRenderTime * 2.0;
        
        // 3. Calculate what is left for the rest of the application (Logic, Physics, Audio)
        // A 90 FPS VR target requires a maximum frame time of 11.11 milliseconds.
        double frameBudget = 11.11;
        double remainingTime = frameBudget - stereoRenderTime;

        UnityEngine.Debug.Log("--- VR Render Budget Estimation (1000 Frames) ---");
        UnityEngine.Debug.Log($"Single Eye Render Time (Average): {averageRenderTime:F3} ms");
        UnityEngine.Debug.Log($"Estimated VR Render Time (x2): {stereoRenderTime:F3} ms");
        UnityEngine.Debug.Log($"Target Frame Budget (90 FPS): {frameBudget} ms");
        
        if (remainingTime > 0)
        {
            UnityEngine.Debug.Log($"Remaining Budget for Logic/Physics: {remainingTime:F3} ms");
        }
        else
        {
            UnityEngine.Debug.Log($"WARNING: Rendering alone exceeds the VR budget by {Mathf.Abs((float)remainingTime):F3} ms. No time left for logic!");
        }
    }
}
```