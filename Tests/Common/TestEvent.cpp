/*
    Copyright (c) 2018-2021 Piotr Doan. All rights reserved.
    Software distributed under the permissive MIT License.
*/

#include <doctest/doctest.h>
#include <Common/Event/Delegate.hpp>
#include <Common/Event/Collector.hpp>
#include <Common/Event/Dispatcher.hpp>
#include <Common/Event/Receiver.hpp>

static const char* Text = "Hello world!";

class Counter
{
public:
    Counter(int* copyCounter = nullptr, int* instanceCounter = nullptr) :
        copyCounter(copyCounter),
        instanceCounter(instanceCounter)
    {
        if(instanceCounter)
        {
            *instanceCounter += 1;
        }
    }

    Counter(const Counter& other) :
        copyCounter(other.copyCounter),
        instanceCounter(other.instanceCounter)
    {
        if(copyCounter)
        {
            *copyCounter += 1;
        }

        if(instanceCounter)
        {
            *instanceCounter += 1;
        }
    }

    Counter(Counter&& other) :
        copyCounter(other.copyCounter),
        instanceCounter(other.instanceCounter)
    {
        if(instanceCounter)
        {
            *instanceCounter += 1;
        }
    }

    ~Counter()
    {
        if(instanceCounter)
        {
            *instanceCounter -= 1;
        }
    }

    Counter& operator=(const Counter& other)
    {
        REQUIRE(&other != this);

        copyCounter = other.copyCounter;

        if(copyCounter)
        {
            *copyCounter += 1;
        }

        return *this;
    }

    Counter& operator=(Counter&& other)
    {
        REQUIRE(&other != this);

        copyCounter = other.copyCounter;
        other.copyCounter = nullptr;

        instanceCounter = other.instanceCounter;
        other.instanceCounter = nullptr;

        return *this;
    }

private:
    int* copyCounter = nullptr;
    int* instanceCounter = nullptr;
};

char Function(Counter instance, int index)
{
    return Text[index];
}

class BaseClass
{
public:
    virtual char Method(Counter instance, int index)
    {
        return Text[index];
    }
};

class DerivedClass : public BaseClass
{
public:
    char Method(Counter instance, int index) override
    {
        return Text[index];
    }
};

TEST_CASE("Event Delegate")
{
    SUBCASE("Binding")
    {
        int argumentCopyCount = 0;
        Counter instance(&argumentCopyCount);

        Event::Delegate<char(Counter instance, int i)> delegate;
        REQUIRE(!delegate.IsBound());

        SUBCASE("Static function binding")
        {
            delegate.Bind<&Function>();
            CHECK(delegate.IsBound());
            CHECK(delegate.Invoke(instance, 4) == 'o');
        }

        SUBCASE("Class method binding")
        {
            BaseClass baseClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            CHECK(delegate.IsBound());
            CHECK(delegate.Invoke(instance, 6) == 'w');
        }

        SUBCASE("Virtual method binding")
        {
            DerivedClass derivedClass;
            delegate.Bind<BaseClass, &BaseClass::Method>(&derivedClass);
            CHECK(delegate.IsBound());
            CHECK(delegate.Invoke(instance, 1) == 'e');
        }

        SUBCASE("Lambda function binding")
        {
            auto functor = [](Counter instance, int index) -> char
            {
                return Text[index];
            };

            delegate.Bind(&functor);
            CHECK(delegate.IsBound());
            CHECK(delegate.Invoke(instance, 10) == 'd');
        }

        SUBCASE("Lambda capture binding via constructor")
        {
            int modifier = 8;
            delegate = Event::Delegate<char(Counter, int)>(
                [&modifier](Counter instance, int index) -> char
                {
                    return Text[index + modifier];
                });

            CHECK(delegate.IsBound());
            CHECK(delegate.Invoke(instance, 3) == '!');
        }

        delegate.Bind(nullptr);
        CHECK(!delegate.IsBound());
        CHECK(argumentCopyCount == 1);
    }

    SUBCASE("Similar lambda signatures")
    {
        int i = 0, y = 0;

        Event::Delegate<void()> delegateOne([&i]() { i = 3; });
        Event::Delegate<void()> delegateTwo([&y]() { y = 7; });

        delegateOne.Invoke();
        delegateTwo.Invoke();

        CHECK(i == 3);
        CHECK(y == 7);
    }

    SUBCASE("Lambda capture lifetime")
    {
        int currentValue = 0, expectedValue = 0;
        int copyCounter = 0, expectedCopies = 0;
        int instanceCounter = 0, expectedInstances = 0;

        Counter instance(&copyCounter, &instanceCounter);
        CHECK(instanceCounter == (expectedInstances += 1));

        Event::Delegate<void()> delegate;

        {
            SUBCASE("Bind lvalue lambda")
            {
                {
                    auto lambda = [&currentValue, instance]()
                    {
                        currentValue += 1;
                    };

                    CHECK(copyCounter == (expectedCopies += 1));
                    CHECK(instanceCounter == (expectedInstances += 1));

                    delegate = lambda;
                    expectedValue += 1;

                    CHECK(copyCounter == (expectedCopies += 1));
                    CHECK(instanceCounter == (expectedInstances += 1));
                }
                
                CHECK(copyCounter == expectedCopies);
                CHECK(instanceCounter == (expectedInstances -= 1));
            }
            
            SUBCASE("Bind rvalue lambda")
            {
                delegate.Bind([&currentValue, instance]()
                    {
                        currentValue += 10;
                    });

                expectedValue += 10;

                CHECK(copyCounter == (expectedCopies += 1));
                CHECK(instanceCounter == (expectedInstances += 1));
            }

            SUBCASE("Copy delegate")
            {
                Event::Delegate<void()> delegateCopy([&currentValue, instance]()
                    {
                        currentValue += 100;
                    });

                expectedValue += 100;

                CHECK(copyCounter == (expectedCopies += 1));
                CHECK(instanceCounter == (expectedInstances += 1));

                delegate = delegateCopy;

                CHECK(copyCounter == (expectedCopies += 1));
                CHECK(instanceCounter == (expectedInstances += 1));

                delegateCopy = nullptr;

                CHECK(copyCounter == (expectedCopies));
                CHECK(instanceCounter == (expectedInstances -= 1));
            }

            SUBCASE("Move delegate")
            {
                Event::Delegate<void()> delegateMove([&currentValue, instance]()
                    {
                        currentValue += 1000;
                    });

                expectedValue += 1000;

                CHECK(copyCounter == (expectedCopies += 1));
                CHECK(instanceCounter == (expectedInstances += 1));

                delegate = std::move(delegateMove);

                CHECK(copyCounter == expectedCopies);
                CHECK(instanceCounter == expectedInstances);

                delegateMove = nullptr;

                CHECK(copyCounter == expectedCopies);
                CHECK(instanceCounter == expectedInstances);
            }

            CHECK(copyCounter == expectedCopies);
            CHECK(instanceCounter == expectedInstances);

            delegate.Invoke();
            CHECK(currentValue == expectedValue);

            CHECK(copyCounter == expectedCopies);
            CHECK(instanceCounter == expectedInstances);

            delegate = nullptr;

            CHECK(copyCounter == expectedCopies);
            CHECK(instanceCounter == (expectedInstances -= 1));
        }

        CHECK(copyCounter == expectedCopies);
        CHECK(instanceCounter == expectedInstances);
    }
}

TEST_CASE("Event Collector")
{
    SUBCASE("Collect default void result")
    {
        Event::CollectDefault<void> collectDefault;

        collectDefault.ConsumeResult();
        CHECK(collectDefault.ShouldContinue() == true);
        collectDefault.GetResult();
    }

    SUBCASE("Collect last result")
    {
        Event::CollectLast<int> collectLast(0);

        for(int i = 0; i < 10; ++i)
        {
            CHECK(collectLast.GetResult() == i);
            collectLast.ConsumeResult(i + 1);
            CHECK(collectLast.ShouldContinue());
            CHECK(collectLast.GetResult() == i + 1);
        }
    }

    SUBCASE("Collect while true")
    {
        Event::CollectWhileTrue collectWhileTrue(true);
        CHECK(collectWhileTrue.GetResult() == true);

        collectWhileTrue.ConsumeResult(true);
        CHECK(collectWhileTrue.ShouldContinue());
        CHECK(collectWhileTrue.GetResult());

        collectWhileTrue.ConsumeResult(false);
        CHECK(!collectWhileTrue.ShouldContinue());
        CHECK(!collectWhileTrue.GetResult());
    }

    SUBCASE("Collect while false")
    {
        Event::CollectWhileFalse collectWhileFalse(false);
        CHECK(collectWhileFalse.GetResult() == false);

        collectWhileFalse.ConsumeResult(false);
        CHECK(collectWhileFalse.ShouldContinue());
        CHECK(!collectWhileFalse.GetResult());

        collectWhileFalse.ConsumeResult(true);
        CHECK(!collectWhileFalse.ShouldContinue());
        CHECK(collectWhileFalse.GetResult());
    }
}

TEST_CASE("Event Dispatcher")
{
    SUBCASE("Collecting last result")
    {
        int i = 0;

        Event::Receiver<int(int&)> receiverAddOne;
        receiverAddOne.Bind([](int& i) { i += 1; return i; });

        Event::Receiver<int(int&)> receiverAddTwo;
        receiverAddTwo.Bind([](int& i) { i += 2; return i; });

        Event::Dispatcher<int(int&)> dispatcher(0);
        CHECK(dispatcher.Dispatch(i) == 0);

        CHECK(dispatcher.Subscribe(receiverAddOne));
        CHECK(dispatcher.Subscribe(receiverAddOne));
        CHECK(dispatcher.Dispatch(i) == 1);

        CHECK(dispatcher.Subscribe(receiverAddTwo));
        CHECK(dispatcher.Subscribe(receiverAddTwo));
        CHECK(dispatcher.Dispatch(i) == 4);

        CHECK(receiverAddOne.Unsubscribe());
        CHECK(!receiverAddOne.Unsubscribe());
        CHECK(dispatcher.Dispatch(i) == 6);

        CHECK(dispatcher.Unsubscribe(receiverAddTwo));
        CHECK(!dispatcher.Unsubscribe(receiverAddTwo));
        CHECK(dispatcher.Dispatch(i) == 0);
    }

    SUBCASE("Collecting boolean result")
    {
        Event::Receiver<bool(int&)> receiverTrue;
        receiverTrue.Bind([](int& i) { i += 1; return true; });

        Event::Receiver<bool(int&)> receiverFalse;
        receiverFalse.Bind([](int& i) { i += 2; return false; });

        Event::Receiver<bool(int&)> receiverDummy;
        receiverDummy.Bind([](int& i) { i += 9999; return true; });
        
        SUBCASE("While returning true")
        {
            int i = 0;

            Event::Dispatcher<bool(int&), Event::CollectWhileTrue> dispatcherWhileTrue(true);
            CHECK(dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 0);

            CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            CHECK(dispatcherWhileTrue.Subscribe(receiverTrue));
            CHECK(dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 1);

            CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            CHECK(dispatcherWhileTrue.Subscribe(receiverFalse));
            CHECK(!dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 4);

            CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            CHECK(dispatcherWhileTrue.Subscribe(receiverDummy));
            CHECK(!dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 7);
        }

        SUBCASE("While returning false")
        {
            int i = 0;

            Event::Dispatcher<bool(int&), Event::CollectWhileFalse> dispatcherWhileFalse(false);
            CHECK(!dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 0);

            CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            CHECK(dispatcherWhileFalse.Subscribe(receiverFalse));
            CHECK(!dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 2);

            CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            CHECK(dispatcherWhileFalse.Subscribe(receiverTrue));
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 5);

            CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            CHECK(dispatcherWhileFalse.Subscribe(receiverDummy));
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 8);
        }

        SUBCASE("Stopped by initial false")
        {
            int i = 0;

            Event::Dispatcher<bool(int&), Event::CollectWhileTrue> dispatcherWhileTrue(false);
            CHECK(!dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 0);

            dispatcherWhileTrue.Subscribe(receiverTrue);
            CHECK(!dispatcherWhileTrue.Dispatch(i));
            CHECK(i == 0);
        }

        SUBCASE("Stopped by initial true")
        {
            int i = 0;

            Event::Dispatcher<bool(int&), Event::CollectWhileFalse> dispatcherWhileFalse(true);
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 0);

            dispatcherWhileFalse.Subscribe(receiverFalse);
            CHECK(dispatcherWhileFalse.Dispatch(i));
            CHECK(i == 0);
        }

        CHECK(!receiverTrue.IsSubscribed());
        CHECK(!receiverFalse.IsSubscribed());
        CHECK(!receiverDummy.IsSubscribed());
    }

    SUBCASE("Receiver subscription")
    {
        int currentA = 0, expectedA = 0, incrementA = 0;
        int currentB = 0, expectedB = 0, incrementB = 0;

        Event::Receiver<void(int&)> receiverAddOne;
        receiverAddOne.Bind([](int& i) { i += 1; });

        Event::Receiver<void(int&)> receiverAddTwo;
        receiverAddTwo.Bind([](int& i) { i += 2; });

        Event::Receiver<void(int&)> receiverAddThree;
        receiverAddThree.Bind([](int& i) { i += 3; });

        Event::Receiver<void(int&)> receiverAddFour;
        receiverAddFour.Bind([](int& i) { i += 4; });

        Event::Dispatcher<void(int&)> dispatcherA;
        Event::Dispatcher<void(int&)> dispatcherB;

        auto dispatchAndCheck = [&]()
        {
            dispatcherA.Dispatch(currentA);
            CHECK(currentA == (expectedA += incrementA));

            dispatcherB.Dispatch(currentB);
            CHECK(currentB == (expectedB += incrementB));
        };

        SUBCASE("Subcribe receivers")
        {
            CHECK(dispatcherA.Subscribe(receiverAddOne));
            CHECK(dispatcherA.Subscribe(receiverAddTwo));
            CHECK(dispatcherA.Subscribe(receiverAddThree));
            CHECK(dispatcherA.Subscribe(receiverAddFour));
            incrementA = 10;

            dispatchAndCheck();

            SUBCASE("Repeat subscription")
            {
                CHECK(dispatcherA.Subscribe(receiverAddOne));
                CHECK(dispatcherA.Subscribe(receiverAddTwo));
                CHECK(dispatcherA.Subscribe(receiverAddThree));
                CHECK(dispatcherA.Subscribe(receiverAddFour));

                dispatchAndCheck();
            }

            SUBCASE("Unsubcribe nonsubscribed")
            {
                CHECK(!dispatcherB.Subscribe(receiverAddOne));
                CHECK(!dispatcherB.Subscribe(receiverAddTwo));
                CHECK(!dispatcherB.Subscribe(receiverAddThree));
                CHECK(!dispatcherB.Subscribe(receiverAddFour));

                dispatchAndCheck();
            }

            SUBCASE("Replace subscription")
            {
                CHECK(dispatcherB.Subscribe(receiverAddOne, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 1;
                incrementB += 1;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddThree, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 3;
                incrementB += 3;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddFour, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 4;
                incrementB += 4;

                dispatchAndCheck();

                CHECK(dispatcherB.Subscribe(receiverAddTwo, Event::SubscriptionPolicy::ReplaceSubscription));
                incrementA -= 2;
                incrementB += 2;

                dispatchAndCheck();

                CHECK(!dispatcherA.Subscribe(receiverAddOne, Event::SubscriptionPolicy::RetainSubscription));
                CHECK(!dispatcherA.Subscribe(receiverAddTwo, Event::SubscriptionPolicy::RetainSubscription));
                CHECK(!dispatcherA.Subscribe(receiverAddThree, Event::SubscriptionPolicy::RetainSubscription));
                CHECK(!dispatcherA.Subscribe(receiverAddFour, Event::SubscriptionPolicy::RetainSubscription));

                dispatchAndCheck();
            }
        }

        dispatchAndCheck();

        dispatcherA.UnsubscribeAll();
        incrementA = 0;

        dispatcherB.UnsubscribeAll();
        incrementB = 0;

        dispatchAndCheck();
    }

    SUBCASE("Subscription change during dispatch")
    {
        int value = 0, firstDispatch = 0, secondDispatch = 0;

        Event::Dispatcher<bool(void), Event::CollectWhileTrue> dispatcher;

        Event::Receiver<bool(void)> receiverFireOnce;
        receiverFireOnce.Bind([&self = receiverFireOnce, &value]()
            {
                self.Unsubscribe();
                value += 1;
                return true;
            });

        Event::Receiver<bool(void)> receiverFireAlways;
        receiverFireAlways.Bind([&value]()
            {
                value += 10;
                return true;
            });

        Event::Receiver<bool(void)> receiverChainA;
        receiverChainA.Bind([&value]()
            {
                value += 100;
                return true;
            });

        Event::Receiver<bool(void)> receiverChainB;
        receiverChainB.Bind([&dispatcher, &receiverChainA, &value]()
            {
                receiverChainA.Subscribe(dispatcher);
                value += 1000;
                return true;
            });

        Event::Receiver<bool(void)> receiverChainC;
        receiverChainC.Bind([&dispatcher, &receiverChainB, &receiverChainC, &value]()
            {
                dispatcher.Subscribe(receiverChainB);
                dispatcher.Unsubscribe(receiverChainC);
                value += 10000;
                return true;
            });

        Event::Receiver<bool(void)> receiverChainD;
        receiverChainD.Bind([&dispatcher, &receiverChainC, &receiverChainD, &value]()
            {
                dispatcher.Unsubscribe(receiverChainD);
                dispatcher.Subscribe(receiverChainC);
                value += 100000;
                return true;
            });

        SUBCASE("Subscribe fire once receiver")
        {
            CHECK(dispatcher.Subscribe(receiverFireOnce));
            firstDispatch += 1;
            secondDispatch += 0;
        }

        SUBCASE("Subscribe fire always receiver")
        {
            CHECK(dispatcher.Subscribe(receiverFireAlways));
            firstDispatch += 10;
            secondDispatch += 10;
        }

        SUBCASE("Susbcribe receiver chain")
        {
            CHECK(dispatcher.Subscribe(receiverChainD));
            firstDispatch += 111100;
            secondDispatch += 1100;
        }

        CHECK(dispatcher.Dispatch());
        CHECK(value == firstDispatch);

        value = 0;
        CHECK(dispatcher.Dispatch());
        CHECK(value == secondDispatch);

        dispatcher.UnsubscribeAll();

        value = 0;
        CHECK(dispatcher.Dispatch());
        CHECK(value == 0);
    }

    SUBCASE("Invoking unbound receivers")
    {
        Event::Dispatcher<int(int&)> dispatcher(0);
        Event::Receiver<int(int&)> receiverUnboundFirst;
        Event::Receiver<int(int&)> receiverUnboundMiddle;
        Event::Receiver<int(int&)> receiverUnboundLast;

        Event::Receiver<int(int&)> receiverBoundFirst;
        receiverBoundFirst.Bind([](int& i) { i += 1; return 1; });

        Event::Receiver<int(int&)> receiverBoundSecond;
        receiverBoundSecond.Bind([](int& i) { i += 2; return 2; });

        CHECK(dispatcher.Subscribe(receiverUnboundFirst));
        CHECK(dispatcher.Subscribe(receiverBoundFirst));
        CHECK(dispatcher.Subscribe(receiverUnboundMiddle));
        CHECK(dispatcher.Subscribe(receiverBoundSecond));
        CHECK(dispatcher.Subscribe(receiverUnboundLast));

        int value = 0;
        CHECK(dispatcher.Dispatch(value) == 2);
        CHECK(value == 3);
    }

    SUBCASE("Dispatch copy count")
    {
        int argumentCopyCount = 0;
        Counter instance(&argumentCopyCount);

        Event::Dispatcher<char(Counter, int)> dispatcher('\0');
        Event::Receiver<char(Counter, int)> receiver;
        CHECK(receiver.Subscribe(dispatcher));

        SUBCASE("Function dispatch")
        {
            receiver.Bind<&Function>();
            CHECK(dispatcher.Dispatch(instance, 0) == 'H');
            CHECK(argumentCopyCount == 1);
        }

        SUBCASE("Method dispatch")
        {
            BaseClass baseClass;
            receiver.Bind<BaseClass, &BaseClass::Method>(&baseClass);
            CHECK(dispatcher.Dispatch(instance, 3) == 'l');
            CHECK(argumentCopyCount == 1);
        }

        SUBCASE("Lambda dispatch")
        {
            receiver.Bind([](Counter, int index) { return Text[index]; });
            CHECK(dispatcher.Dispatch(instance, 5) == ' ');
            CHECK(argumentCopyCount == 1);
        }
    }
}