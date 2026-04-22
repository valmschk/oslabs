#include <gtest/gtest.h>
#include "common.h"
#include <windows.h>
#include <string>

#define MAX_MESSAGES 5

struct MockRingBuffer
{
    char messages[MAX_MESSAGES][MAX_MESSAGE_LENGTH];
    int head;
    int tail;
    int capacity;
};

class RingBufferFixture : public ::testing::Test
{
protected:
    MockRingBuffer buffer;
    HANDLE hMutex;
    HANDLE hEmpty;
    HANDLE hFull;

    virtual void SetUp()
    {
        buffer.capacity = MAX_MESSAGES;
        buffer.head = 0;
        buffer.tail = 0;
        memset(buffer.messages, 0, sizeof(buffer.messages));

        hMutex = CreateSemaphoreA(NULL, 1, 1, NULL);
        hEmpty = CreateSemaphoreA(NULL, MAX_MESSAGES, MAX_MESSAGES, NULL);
        hFull = CreateSemaphoreA(NULL, 0, MAX_MESSAGES, NULL);
    }

    virtual void TearDown()
    {
        CloseHandle(hMutex);
        CloseHandle(hEmpty);
        CloseHandle(hFull);
    }

    void sendMessage(const std::string& msg)
    {
        WaitForSingleObject(hEmpty, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        strncpy(buffer.messages[buffer.tail], msg.c_str(), MAX_MESSAGE_LENGTH - 1);
        buffer.messages[buffer.tail][MAX_MESSAGE_LENGTH - 1] = '\0';
        buffer.tail = (buffer.tail + 1) % buffer.capacity;

        ReleaseSemaphore(hMutex, 1, NULL);
        ReleaseSemaphore(hFull, 1, NULL);
    }

    std::string receiveMessage()
    {
        WaitForSingleObject(hFull, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        std::string msg(buffer.messages[buffer.head]);
        memset(buffer.messages[buffer.head], 0, MAX_MESSAGE_LENGTH);
        buffer.head = (buffer.head + 1) % buffer.capacity;

        ReleaseSemaphore(hMutex, 1, NULL);
        ReleaseSemaphore(hEmpty, 1, NULL);
        return msg;
    }
};

TEST_F(RingBufferFixture, FifoOrder)
{
    sendMessage("first");
    sendMessage("second");
    sendMessage("third");

    EXPECT_EQ(receiveMessage(), "first");
    EXPECT_EQ(receiveMessage(), "second");
    EXPECT_EQ(receiveMessage(), "third");
}

TEST_F(RingBufferFixture, WrapAround)
{
    for (int i = 0; i < MAX_MESSAGES; ++i)
    {
        sendMessage("msg" + std::to_string(i));
    }

    EXPECT_EQ(buffer.tail, 0);
    EXPECT_EQ(buffer.head, 0);

    for (int i = 0; i < MAX_MESSAGES; ++i)
    {
        EXPECT_EQ(receiveMessage(), "msg" + std::to_string(i));
    }
}

TEST_F(RingBufferFixture, MessageLengthTruncation)
{
    std::string longMsg = "ThisMessageIsWayTooLongForBuffer";
    sendMessage(longMsg);

    std::string received = receiveMessage();
    EXPECT_EQ(received.substr(0, MAX_MESSAGE_LENGTH - 1), longMsg.substr(0, MAX_MESSAGE_LENGTH - 1));
}