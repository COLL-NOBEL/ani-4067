#include <iostream>

// Function to find frame rate time left for computayion in 
// milliseconds given that 8 milliseconds are used by VR headset hardware
float activeTimeLeft (float FrameRate) {
    return (1000 / FrameRate) - 8;
}

function to 
int main () {
    float StopProgram = 0;

    while (StopProgram == 0) {
        float FrameRate;
        std::cout << "Enter the frame rate: ";
        std::cin >> FrameRate;

        std::cout << "At the frame rate of " << FrameRate << ", your code gets about "
        << activeTimeLeft(FrameRate) << " milliseconds to run and do all necessary calculations\n\n";

        std::cout << "To try another frame rate, enter \"0\", or anything else to close the program: ";
        std::cin >> StopProgram;
    }
}
