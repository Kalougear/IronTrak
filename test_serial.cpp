#include <Arduino.h>

// Minimal Serial1 UART Test
// This ONLY tests PA9/PA10 UART output
// Upload this, open monitor on COM8, press RESET

void setup() {
    // Configure Serial1 explicitly
    Serial1.setRx(PA10);
    Serial1.setTx(PA9);
    Serial1.begin(115200);
    
    // Wait a moment
    delay(500);
    
    // Test messages
    Serial1.println("========================================");
    Serial1.println("MINIMAL UART TEST");
    Serial1.println("If you see this, UART works!");
    Serial1.println("========================================");
}

void loop() {
    static int counter = 0;
    
    Serial1.print("Counter: ");
    Serial1.println(counter++);
    
    delay(1000);
}
