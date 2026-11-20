#ifndef ORDER_H
#define ORDER_H

#include <string>

// Game mein total delivery orders
const int MAX_ORDERS = 6;

// Har order ki information matching arrays mein save hoti hai
std::string orderShopNames[MAX_ORDERS];
std::string orderCustomerNames[MAX_ORDERS];

int orderPickupRows[MAX_ORDERS];
int orderPickupCols[MAX_ORDERS];
int orderDropRows[MAX_ORDERS];
int orderDropCols[MAX_ORDERS];

float orderTimeLimits[MAX_ORDERS];
float orderTimesRemaining[MAX_ORDERS];

int orderBaseRewards[MAX_ORDERS];
bool orderPackagePickedUp[MAX_ORDERS];

// Game ke current score aur order ki information
int orderCount = MAX_ORDERS;
int currentOrderIndex = 0;
int orderScore = 0;
int completedCount = 0;
int failedCount = 0;
int streakCount = 0;

std::string orderStatusBanner;

// Ek order ki tamam details set karta hai
void setOrder(int index, const std::string& shop,
              const std::string& customer,
              int pickupRow, int pickupCol,
              int dropRow, int dropCol,
              float timeLimit, int reward)
{
    orderShopNames[index] = shop;
    orderCustomerNames[index] = customer;

    orderPickupRows[index] = pickupRow;
    orderPickupCols[index] = pickupCol;
    orderDropRows[index] = dropRow;
    orderDropCols[index] = dropCol;

    orderTimeLimits[index] = timeLimit;
    orderTimesRemaining[index] = timeLimit;
    orderBaseRewards[index] = reward;
    orderPackagePickedUp[index] = false;
}

// Naya shift start karta hai aur tamam orders ki values reset karta hai
void resetOrders()
{
    orderCount = MAX_ORDERS;
    currentOrderIndex = 0;
    orderScore = 0;
    completedCount = 0;
    failedCount = 0;
    streakCount = 0;
    orderStatusBanner = "Follow the RED arrow to the Pizza Shop!";

    setOrder(0, "Pizza Shop", "House 15", 2, 4, 6, 15, 100.0f, 100);
    setOrder(1, "Burger Hub", "House 10", 7, 2, 8, 10, 110.0f, 150);
    setOrder(2, "Coffee Corner", "House 18", 12, 10, 8, 18, 100.0f, 200);
    setOrder(3, "Pizza Shop", "House 2", 2, 4, 11, 2, 95.0f, 250);
    setOrder(4, "Burger Hub", "House 17", 7, 2, 13, 17, 90.0f, 300);
    setOrder(5, "Coffee Corner", "House 15", 12, 16, 6, 15, 85.0f, 400);
}

// Check karta hai ke tamam orders complete ho gaye hain ya nahi
bool allOrdersCompleted()
{
    return currentOrderIndex >= orderCount;
}

// Check karta hai ke current order number valid hai ya nahi
bool hasCurrentOrder()
{
    return currentOrderIndex >= 0 &&
           currentOrderIndex < orderCount;
}

// Current order ka timer kam karta hai
void updateOrderTimer(float deltaTime)
{
    if (allOrdersCompleted())
    {
        return;
    }

    orderTimesRemaining[currentOrderIndex] =
        orderTimesRemaining[currentOrderIndex] - deltaTime;

    if (orderTimesRemaining[currentOrderIndex] < 0.0f)
    {
        orderTimesRemaining[currentOrderIndex] = 0.0f;
    }
}

// Check karta hai ke current order ka time khatam hua ya nahi
bool isOrderExpired()
{
    return !allOrdersCompleted() &&
           orderTimesRemaining[currentOrderIndex] <= 0.0f;
}

// Time khatam hone par current order fail karta hai
void failCurrentOrder()
{
    if (allOrdersCompleted())
    {
        return;
    }

    failedCount++;
    streakCount = 0;
    currentOrderIndex++;

    orderStatusBanner = "Order failed! Time is over.";

    if (failedCount >= 3)
        orderStatusBanner = "Game over! Too many failed orders.";
    else if (allOrdersCompleted())
        orderStatusBanner = "All orders finished.";
    else
        orderStatusBanner =
            "Next order! Follow the red arrow to the pickup shop.";
}

// Delivery complete hone par score aur order count update karta hai
void completeCurrentOrder(float multiplier)
{
    if (allOrdersCompleted())
    {
        return;
    }

    const int earnedPoints =
        orderBaseRewards[currentOrderIndex] * multiplier;

    orderScore = orderScore + earnedPoints;
    completedCount++;
    streakCount++;
    currentOrderIndex++;

    if (allOrdersCompleted())
        orderStatusBanner = "Victory! All orders completed.";
    else
        orderStatusBanner =
            "Great job! Follow the red arrow to the next pickup shop.";
}

// Batata hai ke kitne orders abhi baqi hain
int getRemainingOrders()
{
    return orderCount - currentOrderIndex;
}

#endif
