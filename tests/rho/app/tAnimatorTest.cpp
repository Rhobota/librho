#include <rho/app/tAnimator.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace rho;
using std::cout;
using std::endl;


const int kNumRemoveTests = 10;
const int kRemoveTestIterations = 10000;

int gNumPayloads = 0;


class tCoutingPayload : public app::iPayload
{
    public:

        tCoutingPayload() { gNumPayloads++; }
        tCoutingPayload(const tCoutingPayload&) { gNumPayloads++; }
        ~tCoutingPayload() { gNumPayloads--; }
};


class tAnimatable : public app::iAnimatable
{
    public:

        bool stepAnimation(i32 id, refc<app::iPayload> payload,
                u64 starttime, u64 prevtime, u64 currtime)
        {
            return true;
        }
};


void addRemoveTest(const tTest& t)
{
    t.assert(gNumPayloads == 0);

    {
        app::tAnimator animator((rand() % 1000) + 1234);
        t.assert(animator.isConsistent());
        t.assert(animator.numRegistrations() == 0);

        std::vector<app::iAnimatable*>      animatables;
        std::vector<i32>                    ids;
        std::vector< refc<app::iPayload> >  payloads;

        int numAnimatables = (rand() % 100) + 1;
        int numIds = (rand() % 10) + 1;
        int numPayloads = (rand() % 100) + 1;

        for (int i = 0; i < numAnimatables; i++)
            animatables.push_back(new tAnimatable);
        for (int i = 0; i < numIds; i++)
            ids.push_back(rand() % 1000);
        for (int i = 0; i < numPayloads; i++)
            payloads.push_back(refc<app::iPayload>(new tCoutingPayload));

        t.assert(gNumPayloads == numPayloads);

        for (int i = 0; i < kRemoveTestIterations; i++)
        {
            app::iAnimatable*   animatable = animatables[rand()%numAnimatables];
            i32                 id         = ids[rand()%numIds];
            refc<app::iPayload> payload    = payloads[rand()%numPayloads];

            switch (rand() % 9)
            {
                case 0:
                case 1:
                case 2:
                case 3:
                    if (animator.isRegistered(animatable, id))
                    {
                        try
                        {
                            animator.registerNewAnimation(animatable, id, payload);
                            t.fail();
                        }
                        catch (eLogicError& error)
                        {
                        }
                    }
                    else
                    {
                        animator.registerNewAnimation(animatable, id, payload);
                    }
                    break;
                case 4:
                    animator.cancelAnimation(animatable, id);
                    break;
                case 5:
                    animator.cancelAllAnimationsOn(animatable);
                    break;
                case 6:
                    animator.cancelAndRollbackAnimation(animatable, id);
                    break;
                case 7:
                    animator.cancelAndRollbackAllAnimationsOn(animatable);
                    break;
                case 8:
                    t.assert(animator.isConsistent());
                    break;
                default:
                    t.fail();
            }

            t.assert(gNumPayloads == numPayloads);
        }

        t.assert(animator.isConsistent());
        t.assert(animator.numRegistrations() > 0);   // <-- not guaranteed, but very very likely

        for (int i = 0; i < numAnimatables; i++)
            animator.cancelAllAnimationsOn(animatables[i]);

        t.assert(animator.isConsistent());
        t.assert(animator.numRegistrations() == 0);

        for (int i = 0; i < numAnimatables; i++)
            delete animatables[i];

        t.assert(gNumPayloads == numPayloads);
    }

    t.assert(gNumPayloads == 0);
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("add/remove animatable test", addRemoveTest, kNumRemoveTests);

    return 0;
}
