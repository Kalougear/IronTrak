# STM32 Firmware Architecture Patterns
> **Goal:** Choosing the right structure for your application complexity.

## 1. The Super-Loop (Bare Metal)
**Best For:** Simple sensors, LED controllers, basic logic.

### Structure
```cpp
void main() {
    setup();
    while(1) {
        if (flag_1ms) { task_1ms(); flag_1ms = 0; }
        if (flag_100ms) { task_100ms(); flag_100ms = 0; }
    }
}

void SysTick_Handler() {
    ms_ticks++;
    if (ms_ticks % 1 == 0) flag_1ms = 1;
    if (ms_ticks % 100 == 0) flag_100ms = 1;
}
```
**Pros:** Deterministic, simple, zero overhead.
**Cons:** Spaghetti code risk, hard to handle complex async events.

---

---

## 2. Event-Driven (Reactive)
**Best For:** User Interfaces, Communication Bridges, State Machines.

### Structure
Components react to events (Interrupts, Flags) rather than polling.
**Constraint:** Use **Static Ring Buffers** for queues to avoid dynamic allocation (Rule 3).

```cpp
// Static Circular Buffer (No malloc)
#define Q_SIZE 16
Event eventBuffer[Q_SIZE];
RingBuffer<Event, Q_SIZE> eventQueue;

void main() {
    while(1) {
        if (!eventQueue.empty()) {
            Event e = eventQueue.pop();
            switch(e) {
                case BUTTON_PRESS: handleButton(); break;
                case UART_RX: processPacket(); break;
                // ...
            }
        } else {
            __WFI(); // Sleep until interrupt
        }
    }
}
```
**Pros:** Power efficient (sleeps often), decoupled modules.
**Cons:** Long event handlers block other events.

---

## 3. RTOS (Real-Time Operating System)
**Best For:** IoT, Complex stacks (USB/TCP), Graphical UIs, Mixed-criticality.

### Structure
Independent "Threads" or "Tasks" managed by a scheduler (FreeRTOS, ThreadX).
**Constraint:** MUST use **Static Allocation** APIs (`xTaskCreateStatic`, `xQueueCreateStatic`).

```cpp
// Static Task Memory
StackType_t taskStack[128];
StaticTask_t taskBuffer;

void Task_Sensor(void *arg) {
    while(1) {
        readSensor();
        osDelay(10); // Yields CPU
    }
}

// Init
xTaskCreateStatic(Task_Sensor, "Sens", 128, NULL, 1, taskStack, &taskBuffer);
```
**Pros:** True concurrency, modularity, easy integration of complex middleware.
**Cons:** RAM/Flash overhead, context-switch latency, race condition risks (Mutex hell).

---

## 4. State Machine (FSM)
**Best For:** Protocol parsers, Motor control sequences, Menu systems.
**Can live inside any of the above architectures.**

### Pattern (Switch-Case)
```cpp
enum State { IDLE, RUNNING, ERROR };
State currentState = IDLE;

void updateFSM() {
    switch(currentState) {
        case IDLE:
            if (startCmd) {
                enableMotor();
                currentState = RUNNING;
            }
            break;
        case RUNNING:
            if (error) {
                stopMotor();
                currentState = ERROR;
            }
            break;
        // ...
    }
}
```
