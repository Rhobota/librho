#include <rho/ml/tLearnerCommittee.h>


namespace rho
{
namespace ml
{


tLearnerCommittee::tLearnerCommittee(const std::vector< refc<iLearner> >& committee)
    : m_committee(committee),
      m_threadPool(NULL)
{
    if (m_committee.size() == 0)
        throw eInvalidArgument("A committee must be one or more learners.");
}

tLearnerCommittee::tLearnerCommittee(const std::vector< refc<iLearner> >& committee,
                                     u32 threadPoolSize)
    : m_committee(committee),
      m_threadPool(NULL)
{
    if (m_committee.size() == 0)
        throw eInvalidArgument("A committee must be one or more learners.");
    m_threadPool = new sync::tThreadPool(threadPoolSize);
}

tLearnerCommittee::~tLearnerCommittee()
{
    delete m_threadPool;
    m_threadPool = NULL;
}

static
void s_accum(tIO& accum, const tIO& out)
{
    if (accum.size() != out.size())
        throw eLogicError("The learners of a committee must have the same output dimensionality.");
    for (size_t i = 0; i < accum.size(); i++)
        accum[i] += out[i];
}

class tEvalWorker : public sync::iRunnable, public bNonCopyable
{
    public:

        tEvalWorker(const iLearner* learner, const tIO& input)
            : m_learner(learner),
              m_input(input)
        {
        }

        void run()
        {
            m_learner->evaluate(m_input, m_output);
        }

        const tIO& getOutput() const
        {
            return m_output;
        }

    private:

        const iLearner* m_learner;
        const tIO& m_input;
        tIO m_output;
};

void tLearnerCommittee::evaluate(const tIO& input, tIO& output) const
{
    if (m_threadPool)
    {
        std::vector< refc<sync::iRunnable> > runnables;
        std::vector<sync::tThreadPool::tTaskKey> taskKeys;
        for (size_t i = 0; i < m_committee.size(); i++)
        {
            refc<sync::iRunnable> runnable(new tEvalWorker(m_committee[i], input));
            taskKeys.push_back(m_threadPool->push(runnable));
            runnables.push_back(runnable);
        }
        for (size_t i = 0; i < taskKeys.size(); i++)
        {
            m_threadPool->wait(taskKeys[i]);
        }
        for (size_t i = 0; i < runnables.size(); i++)
        {
            sync::iRunnable* runnable = runnables[i];
            tEvalWorker* worker = dynamic_cast<tEvalWorker*>(runnable);
            if (i == 0) output = worker->getOutput();
            else        s_accum(output, worker->getOutput());
        }
    }
    else
    {
        m_committee[0]->evaluate(input, output);
        for (size_t i = 1; i < m_committee.size(); i++)
        {
            tIO outHere;
            m_committee[i]->evaluate(input, outHere);
            s_accum(output, outHere);
        }
        for (size_t i = 0; i < output.size(); i++)
            output[i] /= ((f64)m_committee.size());
    }
}

void tLearnerCommittee::printLearnerInfo(std::ostream& out) const
{
    for (size_t i = 0; i < m_committee.size(); i++)
    {
        out << "Committee member " << i+1 << ":" << std::endl;
        m_committee[i]->printLearnerInfo(out);
        out << std::endl;
    }
}

std::string tLearnerCommittee::learnerInfoString() const
{
    std::string str = "Committee____";
    str += m_committee[0]->learnerInfoString();
    for (size_t i = 1; i < m_committee.size(); i++)
    {
        str += "__+__";
        str += m_committee[i]->learnerInfoString();
    }
    return str;
}

f64 tLearnerCommittee::calculateError(const tIO& output, const tIO& target)
{
    f64 sum = 0.0;
    for (size_t i = 0; i < m_committee.size(); i++)
        sum += m_committee[i]->calculateError(output, target);
    return sum / ((f64)m_committee.size());
}

f64 tLearnerCommittee::calculateError(const std::vector<tIO>& outputs,
                                      const std::vector<tIO>& targets)
{
    f64 sum = 0.0;
    for (size_t i = 0; i < m_committee.size(); i++)
        sum += m_committee[i]->calculateError(outputs, targets);
    return sum / ((f64)m_committee.size());
}

void tLearnerCommittee::addExample(const tIO& input, const tIO& target,
                                         tIO& actualOutput)
{
    throw eLogicError("Do not call this method. You cannot train a committee of learners.");
}

void tLearnerCommittee::update()
{
    throw eLogicError("Do not call this method. You cannot train a committee of learners.");
}

void tLearnerCommittee::reset()
{
    throw eLogicError("Do not call this method. You cannot train a committee of learners.");
}


}   // namespace ml
}   // namespace rho
