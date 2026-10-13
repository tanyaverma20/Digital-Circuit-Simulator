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
    components[0].setPosition(sf::Vector2f(posx, posy - compat::rectHeight(components[0].getLocalBounds()) + offsetY));
    for (size_t i = 1; i < components.size(); i++)
    {
        sf::Vector2f pos = components[i - 1].getPosition();
        components[i].setPosition(sf::Vector2f(pos.x + compat::rectWidth(components[i - 1].getLocalBounds()) / 2 + offsetX,
                                               posy - compat::rectHeight(components[i].getLocalBounds()) + offsetY));
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
    std::ofstream file("history.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open history.txt for writing" << std::endl;
        return;
    }

    file << "# DIGITAL CIRCUIT SIMULATOR SAVE FILE V2\n";

    // Save Gates
    file << "GATES " << Gate::gates.size() << "\n";
    for (auto gate : Gate::gates)
    {
        file << gate->getType() << " " << gate->getPosition().x << " " << gate->getPosition().y << "\n";
    }

    // Save Input Signals
    file << "INPUTS " << InputSignal::signals.size() << "\n";
    for (auto signal : InputSignal::signals)
    {
        file << signal->getPosition().x << " " << signal->getPosition().y << " " << (signal->getState() ? 1 : 0) << "\n";
    }

    // Save Output Signals
    file << "OUTPUTS " << OutputSignal::signals.size() << "\n";
    for (auto signal : OutputSignal::signals)
    {
        file << signal->getPosition().x << " " << signal->getPosition().y << "\n";
    }

    // Save Connections
    file << "CONNECTIONS " << Connection::connections.size() << "\n";
    for (auto connection : Connection::connections)
    {
        file << connection->ip->getPosition().x << " " << connection->ip->getPosition().y << " "
             << connection->op->getPosition().x << " " << connection->op->getPosition().y << "\n";
    }

    file.close();
    std::cout << "Circuit saved successfully (" << Gate::gates.size() << " gates, "
              << Connection::connections.size() << " connections)." << std::endl;
}

void Component::load()
{
    std::ifstream file("history.txt");
    if (!file.is_open())
    {
        std::cerr << "Failed to open history.txt for reading" << std::endl;
        return;
    }

    for (auto con : Connection::connections)
    {
        delete con;
    }
    Connection::connections.clear();

    Gate::removeAll();
    OutputSignal::removeAll();
    InputSignal::removeAll();

    std::string tag;
    while (file >> tag)
    {
        if (tag == "#")
        {
            std::string comment;
            std::getline(file, comment);
        }
        else if (tag == "GATES")
        {
            size_t count = 0;
            file >> count;
            for (size_t i = 0; i < count; ++i)
            {
                std::string type;
                float x, y;
                file >> type >> x >> y;
                Gate::gates.push_back(new Gate(type, sf::Vector2f(x, y)));
            }
        }
        else if (tag == "INPUTS")
        {
            size_t count = 0;
            file >> count;
            for (size_t i = 0; i < count; ++i)
            {
                float x, y;
                int stateInt = 0;
                file >> x >> y >> stateInt;
                auto sig = new InputSignal(sf::Vector2f(x, y));
                sig->setState(stateInt != 0);
                InputSignal::signals.push_back(sig);
            }
        }
        else if (tag == "OUTPUTS")
        {
            size_t count = 0;
            file >> count;
            for (size_t i = 0; i < count; ++i)
            {
                float x, y;
                file >> x >> y;
                OutputSignal::signals.push_back(new OutputSignal(sf::Vector2f(x, y)));
            }
        }
        else if (tag == "CONNECTIONS")
        {
            size_t count = 0;
            file >> count;
            for (size_t i = 0; i < count; ++i)
            {
                float ipX, ipY, opX, opY;
                file >> ipX >> ipY >> opX >> opY;
                InputPort *ip = findInputPortAtPosition(sf::Vector2f(ipX, ipY));
                OutputPort *op = findOutputPortAtPosition(sf::Vector2f(opX, opY));
                if (ip && op)
                {
                    Connection::connections.push_back(new Connection(ip, op));
                }
            }
        }
        else
        {
            // Legacy format fallback: <type> <x> <y>
            float x = 0.f, y = 0.f;
            if (file >> x >> y)
            {
                Gate::gates.push_back(new Gate(tag, sf::Vector2f(x, y)));
            }
        }
    }

    file.close();
    std::cout << "Circuit loaded successfully (" << Gate::gates.size() << " gates, "
              << Connection::connections.size() << " connections)." << std::endl;
}

InputPort *Component::findInputPortAtPosition(sf::Vector2f pos)
{
    const float threshold = 15.0f;
    for (auto gate : Gate::gates)
    {
        for (auto &ip : gate->iPorts)
        {
            float dx = ip.getPosition().x - pos.x;
            float dy = ip.getPosition().y - pos.y;
            if (std::sqrt(dx * dx + dy * dy) < threshold)
                return &ip;
        }
    }
    for (auto sig : OutputSignal::signals)
    {
        float dx = sig->getPosition().x - pos.x;
        float dy = sig->getPosition().y - pos.y;
        if (std::sqrt(dx * dx + dy * dy) < threshold)
            return sig;
    }
    return nullptr;
}

OutputPort *Component::findOutputPortAtPosition(sf::Vector2f pos)
{
    const float threshold = 15.0f;
    for (auto gate : Gate::gates)
    {
        float dx = gate->oPort.getPosition().x - pos.x;
        float dy = gate->oPort.getPosition().y - pos.y;
        if (std::sqrt(dx * dx + dy * dy) < threshold)
            return &gate->oPort;
    }
    for (auto sig : InputSignal::signals)
    {
        float dx = sig->getPosition().x - pos.x;
        float dy = sig->getPosition().y - pos.y;
        if (std::sqrt(dx * dx + dy * dy) < threshold)
            return sig;
    }
    return nullptr;
}

