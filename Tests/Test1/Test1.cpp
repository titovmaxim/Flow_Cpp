#include <iostream>
#include <thread>
#include "../../Libraries/Unicore.Library/Utils.h"
#include "../../Libraries/Unicore.Library/Flow.h"
#include "../../Libraries/Unicore.Library/Timer.h"

using namespace Unicore::Library;


Flow::SourcePtr<int> CreateFunc()
{
    auto const0 = Flow::Const(2);

    auto var1 = Flow::Var(2);

    auto prop2Field = 3;
    auto prop2 = Flow::Property<int>(
        [&prop2Field]() { return prop2Field; },
        [&prop2Field](auto value) { prop2Field = value; }
    );

    auto func0 = const0->Func<int>([](auto x) { return 55 * x; });

    auto intFunc1 = var1->Func<double>([](auto x) { return 0.1 + x; });
    // Exactly the same as: auto intFunc1 = Flow::Func<int, double>(var1, [](auto x) { return 0.1 + x; });

    auto connector1 = Flow::Connector<double>(-100.0);
    connector1->ConnectTo(intFunc1);

    auto func2 = Flow::Func<double, int, int>(var1, prop2, [](auto x1, auto x2) { return 0.1 + x1 * x2; });

    auto func3 = func2
        ->Cache()
        ->Func<int>([](auto x) -> int { return (int) (x / 100); })
        ->FilterChanges();

    std::cout << func0->Value() << std::endl;
    std::cout << connector1->Value() << std::endl;
    std::cout << func2->Value() << std::endl;

    {
        std::cout << "==== Connector ====" << std::endl;

        connector1->Disconnect();
        std::cout << connector1->Value() << std::endl;

        connector1->ConnectTo(intFunc1);
        std::cout << connector1->Value() << std::endl;
    }

    {
        std::cout << "==== Callbacks ====" << std::endl;

        auto callback = Flow::Callback([]() {
            std::cout << "Callback" << std::endl;
        });
        connector1->AddSubscription(callback);
        func2->AddSubscription(callback);

        var1->SetValue(10); // 2x Callback (fun1, func2)
        prop2->SetValue(20); // 1x Callback (only func2)
    }

    {
        std::cout << "==== Actions ====" << std::endl;

        Flow::LifetimeOwner owner{};

        var1->Action(&owner, [](auto v1) {
            std::cout << "Action 1" << std::endl;
        });

        Flow::Action<int, int>(&owner, var1, func3, [](auto v1, auto f3) {
            std::cout << "Action 2" << std::endl;
        });

        var1->SetValue(100);

        owner.ReleaseOwned(); // Should be done in destructor of real class
    }

    {
        std::cout << "==== Func3 (FilterChanges) ====" << std::endl;

        auto action = Flow::Callback([]() {
            std::cout << "Real change detected" << std::endl;
        });
        func3->AddSubscription(action);

        std::cout << func3->Value() << std::endl;
        var1->SetValue(12);
        std::cout << func3->Value() << std::endl;
        var1->SetValue(14);
        std::cout << func3->Value() << std::endl;
        var1->SetValue(16);
        std::cout << func3->Value() << std::endl;
        var1->SetValue(18);
        std::cout << func3->Value() << std::endl;
        var1->SetValue(20);
        std::cout << func3->Value() << std::endl;
    }

//    {
//        std::cout << "==== Ptrs comparison ====" << std::endl;
//
//        std::cout << var1.use_count() << std::endl;
//        {
//            auto ptr1 = var1->SharedPtr<Flow::IChangeable>();
//            std::cout << var1.use_count() << std::endl;
//            {
//                auto ptr2 = var1;
//                std::cout << var1.use_count() << std::endl;
//                {
//                    auto ptr3 = ptr1;
//                    std::cout << var1.use_count() << std::endl;
//                }
//            }
//        }
//        std::cout << var1.use_count() << std::endl;
//    }

    return func3;
}

int main()
{
    auto func3 = CreateFunc();
    {
        std::cout << "==== Test func3 pass (connections lifetime) ====" << std::endl;
        std::cout << func3->Value() << std::endl;
    }

    {
        std::cout << "==== Timer test ====" << std::endl;

        Timer timer(0.250, []() {
            std::cout << "-> Timer" << std::endl;
        });
        timer.Start();

        std::cout << "Is active: " << timer.IsActive() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        timer.Stop();

        std::cout << "Is active: " << timer.IsActive() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        timer.Start();

        std::cout << "Is active: " << timer.IsActive() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    {
        std::cout << "==== Throttle test ====" << std::endl;

        Flow::LifetimeOwner owner{};

        auto var1 = Flow::Var(0);

        var1->Func<double>([](auto x) { return x * 50.0 / 1000.0; })
            ->Throttle(0.250)
            ->Action(&owner, [](auto x) {
                std::cout << "OnChange: " << x << std::endl;
            });

        for (int i = 20; i-- > 0;) {
            var1->SetValue(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::cout << "Before sleep: " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "After sleep: " << std::endl;

        for (int i = 20; i-- > 0;) {
            var1->SetValue(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        std::cout << "Before sleep: " << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "After sleep: " << std::endl;

        owner.ReleaseOwned(); // Should be done in destructor of real class
    }
}
