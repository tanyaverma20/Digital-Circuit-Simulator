#include "../header_files/components.hpp"

std::vector<Button> Component::components;

Component::Component(sf::RenderWindow &window)
{
    sf::Vector2u bounds = window.getSize();
    posx = bounds.x / 2 + 150;
    posy = bounds.y;
    offsetX = 50.f;
    offsetY = 10.f;

    components.push_back(Button("AND", sf::Vector2f(posx, posy)));
    components.push_back(Button("OR", sf::Vector2f(posx, posy)));
    components.push_back(Button("NOT", sf::Vector2f(posx, posy)));
    components.push_back(Button("NAND", sf::Vector2f(posx, posy)));
    components.push_back(Button("NOR", sf::Vector2f(posx, posy)));
    components.push_back(Button("XOR", sf::Vector2f(posx, posy)));
    components.push_back(Button("XNOR", sf::Vector2f(posx, posy)));
    components.push_back(Button("INPUT", sf::Vector2f(posx, posy)));
    components.push_back(Button("OUTPUT", sf::Vector2f(posx, posy)));
    components.push_back(Button("SAVE", sf::Vector2f(posx, posy)));
    components.push_back(Button("LOAD", sf::Vector2f(posx, posy)));

    setPosition();
}

void Component::setPosition()
{
    components[0].setPosition(sf::Vector2f(posx, posy - components[0].getLocalBounds().height + offsetY));
    for (size_t i = 1; i < components.size(); i++)
    {
        sf::Vector2f pos = components[i - 1].getPosition();
        components[i].setPosition(sf::Vector2f(pos.x + components[i - 1].getLocalBounds().width / 2 + offsetX, posy - components[i].getLocalBounds().height + offsetY));
    }
}

void Component::drawTo(sf::RenderWindow &window)
{
    for (auto &component : components)
    {
        component.drawTo(window);
    }
}

int Component::newComponent(Button comp, sf::Vector2f pos)
{
    std::cout << "Creating new component: " << comp.getText() << " at position (" << pos.x << ", " << pos.y << ")\n";

    if (comp.getText() == "AND")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_AND, pos));
        return 0;
    }
    else if (comp.getText() == "OR")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_OR, pos));
        return 0;
    }
    else if (comp.getText() == "NOT")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_NOT, pos));
        return 0;
    }
    else if (comp.getText() == "NAND")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_NAND, pos));
        return 0;
    }
    else if (comp.getText() == "NOR")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_NOR, pos));
        return 0;
    }
    else if (comp.getText() == "XOR")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_XOR, pos));
        return 0;
    }
    else if (comp.getText() == "XNOR")
    {
        Gate::gates.push_back(new Gate(Gate::LOGIC_XNOR, pos));
        return 0;
    }
    else if (comp.getText() == "INPUT")
    {
        InputSignal::signals.push_back(new InputSignal(pos));
        return 1;
    }
    else if (comp.getText() == "OUTPUT")
    {
        OutputSignal::signals.push_back(new OutputSignal(pos));
        return 2;
    }
    else if (comp.getText() == "SAVE")
    {
        save();
        return 3;
    }
    else if (comp.getText() == "LOAD")
    {
        load();
        return 4;
    }
    return 5;
}

void Component::save()
{
    std::ofstream file;
    file.open("history.txt");
    if (!file.is_open())
    {
        std::cout << "failed to save" << std::endl;
        return;
    }

    for (auto gate : Gate::gates)
    {
        file << gate->getType() << " " << gate->getPosition().x << " " << gate->getPosition().y << std::endl;
    }

    // file << std::endl;
    // for (auto connection : Connection::connections)
    // {
    //     file << connection->ip->getPosition().x << " " << connection->ip->getPosition().y << " "
    //          << connection->op->getPosition().x << " " << connection->op->getPosition().y << std::endl;
    // }

    file.close();
}

void Component::load()
{
    std::ifstream file;
    file.open("history.txt");
    if (!file.is_open())
    {
        std::cout << "failed to load" << std::endl;
        return;
    }

    Gate::removeAll();
    OutputSignal::removeAll();
    InputSignal::removeAll();

    std::string type;
    float posX, posY;
    while (file >> type >> posX >> posY)
    {
        Gate *gate = new Gate(type, sf::Vector2f(posX, posY));
        Gate::gates.push_back(gate);
    }

    // float ipPosX, ipPosY, opPosX, opPosY;
    // while (file >> ipPosX >> ipPosY >> opPosX >> opPosY)
    // {
    //     InputPort *ip = findInputPortAtPosition(sf::Vector2f(ipPosX, ipPosY));
    //     OutputPort *op = findOutputPortAtPosition(sf::Vector2f(opPosX, opPosY));
    //     if (ip && op)
    //     {
    //         Connection::connections.push_back(new Connection(ip, op));
    //     }
    // }

    file.close();
}

InputPort *Component::findInputPortAtPosition(sf::Vector2f pos)
{
    for (auto gate : Gate::gates)
    {
        for (auto &ip : gate->iPorts)
        {
            if (ip.getPosition() == pos)
                return &ip;
        }
    }
    return nullptr;
}

OutputPort *Component::findOutputPortAtPosition(sf::Vector2f pos)
{
    for (auto gate : Gate::gates)
    {
        if (gate->oPort.getPosition() == pos)
            return &gate->oPort;
    }
    return nullptr;
}
