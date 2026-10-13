#include "../header_files/outputSignal.hpp"

std::vector<OutputSignal *> OutputSignal::signals;

OutputSignal::OutputSignal(sf::Vector2f pos)
{
    this->setRadius(9.f);
    this->setOrigin(this->getRadius(), this->getRadius());
    this->setOutlineColor(sf::Color::Red);
    this->setOutlineThickness(1.7f);
    this->setPosition(pos);
}

OutputSignal::~OutputSignal()
{
    delete this;
}

void OutputSignal::deleteConnections()
{

    for (auto conIt = Connection::connections.begin(); conIt != Connection::connections.end();)
    {
        auto con = *conIt;

        if (con->ip == this)
        {
            for (auto it = parentPort->childPorts.begin(); it != parentPort->childPorts.end();)
            {
                auto port = *it;
                if (port == this)
                {
                    it = parentPort->childPorts.erase(it);
                    break;
                }
                ++it;
            }
            conIt = Connection::connections.erase(conIt);
            //
            this->isConnected = false;
            this->setState(false);
            break;
        }
        ++conIt;
    }
}

void OutputSignal::removeAtPos(sf::Vector2f pos)
{

    for (auto it = signals.begin(); it != signals.end();)
    {
        auto sig = *it;

        if (sig->contains(pos))
        {
            sig->deleteConnections();
            it = signals.erase(it);
            break;
        }
        ++it;
    }
}

void OutputSignal::removeAll()
{
    for (auto signal : signals)
    {
        signal->deleteConnections();
    }
    signals.clear();
}

void OutputSignal::duplicate()
{
    OutputSignal *dup = new OutputSignal(this->getPosition());
    dup->parentPort = this->parentPort;
    signals.push_back(dup);
}