#include <rho/app/tDispatcher.h>
#include <rho/tCrashReporter.h>
#include <rho/tTest.h>

#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace rho;


const int kNumRemoveTests = 10;
const int kRemoveTestIterations = 1000;


class tSource : public app::iSource
{
    public:
};


class tTarget : public app::iTarget
{
    public:

        void notify(app::iSource* source, app::tNotificationCode code, app::iPayload* payload)
        {
        }
};


app::tNotificationCode genRandomCode()
{
    app::tNotificationCode code;
    int s = rand() % 100;
    for (int i = 0; i < s; i++)
        code += (rand() % 26) + 'a';
    return code;
}


void addRemoveTest(const tTest& t)
{
    app::tDispatcher disp;
    t.assert(disp.isConsistent());
    t.assert(disp.numRegistrations() == 0);

    std::vector<app::iSource*> sources;
    std::vector<app::iTarget*> targets;
    std::vector<app::tNotificationCode> codes;

    int numSources = (rand() % 1000) + 1;
    int numTargets = (rand() % 1000) + 1;
    int numCodes = (rand() % 1000) + 1;

    for (int i = 0; i < numSources; i++)
        sources.push_back(new tSource);
    for (int i = 0; i < numTargets; i++)
        targets.push_back(new tTarget);
    for (int i = 0; i < numCodes; i++)
        codes.push_back(genRandomCode());

    for (int i = 0; i < kRemoveTestIterations; i++)
    {
        app::iSource* src = sources[rand()%numSources];
        app::iTarget* tar = targets[rand()%numTargets];
        app::tNotificationCode code = codes[rand()%numCodes];

        switch (rand() % 11)
        {
            case 0:
            case 1:
                disp.addTargetToSource(tar, src);
                break;
            case 2:
            case 3:
                disp.addTargetToCode(tar, code);
                break;
            case 4:
            case 5:
                disp.addTargetToSourceCodePair(tar, src, code);
                break;
            case 6:
                disp.removeTarget(tar);
                break;
            case 7:
                disp.removeTargetFromSource(tar, src);
                break;
            case 8:
                disp.removeTargetFromCode(tar, code);
                break;
            case 9:
                disp.removeTargetFromSourceCodePair(tar, src, code);
                break;
            case 10:
                t.assert(disp.isConsistent());
                break;
            default:
                t.fail();
        }
    }

    t.assert(disp.isConsistent());
    t.assert(disp.numRegistrations() > 0);

    for (int i = 0; i < numTargets; i++)
        disp.removeTarget(targets[i]);

    t.assert(disp.isConsistent());
    t.assert(disp.numRegistrations() == 0);

    for (int i = 0; i < numSources; i++)
        delete sources[i];
    for (int i = 0; i < numTargets; i++)
        delete targets[i];
}


int main()
{
    tCrashReporter::init();
    srand(time(0));

    tTest("add/remove target test", addRemoveTest, kNumRemoveTests);

    return 0;
}
