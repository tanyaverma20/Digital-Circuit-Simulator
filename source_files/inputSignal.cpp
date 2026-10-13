#include "../header_files/inputSignal.hpp"

std::vector<InputSignal *> InputSignal::signals;
InputSignal *InputSignal::selected = nullptr;

InputSignal::InputSignal(sf::Vector2f pos)
{
    this->setRadius(9.f);
    this->setOrigin(this->getRadius(), this->getRadius());
    this->setOutlineColor(sf::Color::Blue);
    this->setOutlineThickness(1.7f);
    this->setPosition(pos);
}

InputSignal::~InputSignal()
{
    delete this;
}

void InputSignal::deleteConnections()
{

    for (auto conIt = Connection::connections.begin(); conIt != Connection::connections.end();)
    {
        auto con = *conIt;

        if (con->op == this)
        {
            conIt = Connection::connections.erase(conIt);
        }
        else
            ++conIt;
    }

    for (auto child : childPorts)
    {
        child->parentPort = nullptr;
        child->isConnected = false;
        child->setState(false);
    }
    childPorts.clear();
}

void InputSignal::removeAtPos(sf::Vector2f pos)
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

void InputSignal::removeAll()
{
    for (auto signal : signals)
    {
        signal->deleteConnections();
    }
    signals.clear();
}

void InputSignal::duplicate()
{
    InputSignal *dup = new InputSignal(this->getPosition());
    dup->setState(this->getState());
    signals.push_back(dup);
}
