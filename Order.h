#ifndef ORDER_H
#define ORDER_H

#include <string>

// Order list and status tracking
const int MAX_ORDERS = 6;
static std::string orderShopNames[MAX_ORDERS];
static std::string orderCustomerNames[MAX_ORDERS];
static int orderPickupRows[MAX_ORDERS];
static int orderPickupCols[MAX_ORDERS];
static int orderDropRows[MAX_ORDERS];
static int orderDropCols[MAX_ORDERS];
static float orderTimeLimits[MAX_ORDERS];
static float orderTimesRemaining[MAX_ORDERS];
static int orderBaseRewards[MAX_ORDERS];
static bool orderPackagePickedUp[MAX_ORDERS];

static int orderCount = MAX_ORDERS;
static int currentOrderIndex = 0;
static int orderScore = 0;
static int completedCount = 0;
static int failedCount = 0;
static int streakCount = 0;
static std::string orderStatusBanner;

static void setOrder(int index, const std::string& shop,
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

static void resetOrders()
{
    orderCount = MAX_ORDERS;
    currentOrderIndex = 0;
    orderScore = 0;
    completedCount = 0;
    failedCount = 0;
    streakCount = 0;
    orderStatusBanner = "Drive to the GREEN Pickup Marker (Pizza Shop)!";

    setOrder(0, "Pizza Shop", "House 15", 2, 4, 6, 15, 100.0f, 100);
    setOrder(1, "Burger Hub", "House 10", 7, 2, 8, 10, 110.0f, 150);
    setOrder(2, "Coffee Corner", "House 18", 12, 10, 8, 18, 100.0f, 200);
    setOrder(3, "Pizza Shop", "House 2", 2, 4, 11, 2, 95.0f, 250);
    setOrder(4, "Burger Hub", "House 17", 7, 2, 13, 17, 90.0f, 300);
    setOrder(5, "Coffee Corner", "House 15", 12, 16, 6, 15, 85.0f, 400);
}

static bool allOrdersCompleted()
{
    return currentOrderIndex >= orderCount;
}

static bool hasCurrentOrder()
{
    return currentOrderIndex >= 0 && currentOrderIndex < orderCount;
}

static void updateOrderTimer(float deltaTime)
{
    if (allOrdersCompleted()) return;
    orderTimesRemaining[currentOrderIndex] -= deltaTime;
    if (orderTimesRemaining[currentOrderIndex] < 0.0f)
        orderTimesRemaining[currentOrderIndex] = 0.0f;
}

static bool isOrderExpired()
{
    return !allOrdersCompleted() &&
           orderTimesRemaining[currentOrderIndex] <= 0.0f;
}

static void failCurrentOrder()
{
    if (allOrdersCompleted()) return;
    failedCount++;
    streakCount = 0;
    currentOrderIndex++;
    orderStatusBanner = "Order Failed! Time is over.";

    if (failedCount >= 3)
        orderStatusBanner = "GAME OVER! Too many failed orders.";
    else if (allOrdersCompleted())
        orderStatusBanner = "All Orders Finished!";
    else
        orderStatusBanner = "Next order! Find the pickup shop.";
}

static void completeCurrentOrder(float multiplier)
{
    if (allOrdersCompleted()) return;
    const int earnedPoints = orderBaseRewards[currentOrderIndex] * multiplier;
    orderScore += earnedPoints;
    completedCount++;
    streakCount++;
    currentOrderIndex++;
    orderStatusBanner = allOrdersCompleted()
        ? "VICTORY! All Orders Completed."
        : "Great Job! Find the next pickup shop.";
}

static int getRemainingOrders()
{
    return orderCount - currentOrderIndex;
}

#endif
