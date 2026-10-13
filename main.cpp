#include "./header_files/main.hpp"

sf::Vector2f getMousePos(sf::RenderWindow &);

void mainLoop(sf::RenderWindow &window)
{
    Component components(window);

    StatusBar status(FontManager::font);
    status.setPosition(sf::Vector2f(20.f, window.getSize().y - status.getGlobalBounds().height * 2.f));

    bool isConnecting = false;
    sf::Vector2f startPoint;

    sf::RectangleShape currentConnection;

    OutputPort *prevPort = nullptr;
    Gate *selectedGate = nullptr;
    Port *selectedPort = nullptr;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                Gate::removeAll();
                OutputSignal::removeAll();
                InputSignal::removeAll();

                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::C)
                {
                    for (auto &gate : Gate::gates)
                    {
                        gate->deleteConnections();
                        std::cout << "deleted connection" << std::endl;
                    }
                }
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::X)
                {
                    Gate::removeAll();
                    OutputSignal::removeAll();
                    InputSignal::removeAll();
                }
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Right)
                {
                    sf::Vector2f mousePos = getMousePos(window);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    {
                        bool connDeleted = false;
                        for (auto gate : Gate::gates)
                        {
                            if (gate->contains(mousePos))
                            {
                                gate->deleteConnections();
                                connDeleted = true;
                                break;
                            }
                        }

                        if (!connDeleted)
                            for (auto signal : OutputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    signal->deleteConnections();
                                    connDeleted = true;
                                    break;
                                }
                            }
                        if (!connDeleted)
                            for (auto signal : InputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    signal->deleteConnections();
                                    break;
                                }
                            }
                    }

                    else
                    {
                        Gate::removeAtPos(mousePos);
                        OutputSignal::removeAtPos(mousePos);
                        InputSignal::removeAtPos(mousePos);
                    }
                }

                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f mousePos = getMousePos(window);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                    {
                        for (auto signal : InputSignal::signals)
                        {
                            if (signal->contains(mousePos))
                            {
                                selectedPort = signal;
                                break;
                            }
                        }
                    }

                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
                    {
                        for (auto signal : InputSignal::signals)
                        {
                            if (signal->contains(mousePos))
                            {
                                signal->toggleState();
                                break;
                            }
                        }
                    }

                    //

                    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                    {
                        bool mouseContained = false;

                        for (auto &gate : Gate::gates)
                        {
                            if (gate->contains(mousePos))
                            {
                                gate->duplicate(mousePos);
                                selectedGate = Gate::gates.back();
                                mouseContained = true;
                                break;
                            }
                        }

                        if (!mouseContained)
                            for (auto signal : InputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    signal->duplicate();
                                    selectedPort = InputSignal::signals.back();
                                    mouseContained = true;
                                    break;
                                }
                            }

                        if (!mouseContained)
                            for (auto signal : OutputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    signal->duplicate();
                                    selectedPort = OutputSignal::signals.back();
                                    mouseContained = true;
                                    break;
                                }
                            }
                    }

                    //

                    else
                    {

                        sf::Vector2f mousePos = getMousePos(window);

                        bool mouseContained = false;
                        for (Gate *const gate : Gate::gates)
                        {
                            if (gate->oPort.contains(mousePos))
                            {
                                isConnecting = true;
                                startPoint = gate->oPort.getPosition();

                                currentConnection.setSize(sf::Vector2f(0, 3.f));
                                currentConnection.setOrigin(0, currentConnection.getLocalBounds().height / 2);
                                currentConnection.setPosition(startPoint);
                                currentConnection.setFillColor(gate->oPort.getState() ? sf::Color::Green : sf::Color::White);
                                prevPort = &gate->oPort;
                                mouseContained = true;
                                break;
                            }

                            else if (gate->contains(mousePos))
                            {
                                selectedGate = gate;
                                mouseContained = true;
                                break;
                            }
                        }

                        if (!mouseContained)
                            for (auto signal : InputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    isConnecting = true;
                                    startPoint = signal->getPosition();

                                    currentConnection.setSize(sf::Vector2f(0, 3.f));
                                    currentConnection.setOrigin(0, currentConnection.getLocalBounds().height / 2);
                                    currentConnection.setPosition(startPoint);
                                    currentConnection.setFillColor(signal->getState() ? sf::Color::Green : sf::Color::White);
                                    prevPort = signal;
                                    mouseContained = true;
                                    break;
                                }
                            }
                        if (!mouseContained)

                            for (auto signal : OutputSignal::signals)
                            {
                                if (signal->contains(mousePos))
                                {
                                    selectedPort = signal;
                                    mouseContained = true;
                                    break;
                                }
                            }

                        //

                        if (!mouseContained)
                            for (auto &comp : Component::components)
                            {
                                if (comp.contains(mousePos))
                                {
                                    int compCode = Component::newComponent(comp, mousePos);

                                    if (compCode == 0)
                                        selectedGate = Gate::gates.back();
                                    else if (compCode == 1)
                                        selectedPort = InputSignal::signals.back();
                                    else if (compCode == 2)
                                        selectedPort = OutputSignal::signals.back();
                                    mouseContained = true;
                                    break;
                                }
                            }

                        //
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    selectedGate = nullptr;
                    selectedPort = nullptr;

                    if (isConnecting)
                    {
                        sf::Vector2f mousePos = getMousePos(window);
                        bool escape = false;
                        for (auto gate : Gate::gates)
                        {
                            for (auto &port : gate->iPorts)
                            {
                                if (port.isConnected)
                                {
                                    continue;
                                }

                                if (port.contains(mousePos))
                                {
                                    Connection::connections.push_back(new Connection(&port, prevPort));
                                    escape = true;
                                    break;
                                }
                            }
                            if (escape)
                                break;
                        }
                        //
                        for (auto signal : OutputSignal::signals)
                        {
                            if (signal->isConnected)
                                continue;

                            if (signal->contains(mousePos))
                            {
                                Connection::connections.push_back(new Connection(signal, prevPort));
                                break;
                            }
                        }
                        //
                        isConnecting = false;
                        prevPort = nullptr;
                    }
                }
            }

            if (event.type == sf::Event::MouseMoved)
            {
                for (auto gate : Gate::gates)
                {
                    gate->hoverAction(window);
                }

                if (selectedGate != nullptr)
                {
                    selectedGate->setPosition(getMousePos(window));
                }
                if (selectedPort != nullptr)
                {
                    selectedPort->setPosition(getMousePos(window));
                }
            }
        }
        if (isConnecting)
        {
            sf::Vector2f mousePos = getMousePos(window);
            sf::Vector2f vec = mousePos - startPoint;
            float magnitude = std::sqrt(vec.x * vec.x + vec.y * vec.y);
            float angle = (180.f / 3.141592f) * std::atan2(vec.y, vec.x);
            currentConnection.setSize(sf::Vector2f(magnitude, 3.f));
            currentConnection.setRotation(angle);
        }

        status.setString("gate count: " + std::to_string(Gate::gates.size()));

        window.clear(sf::Color::Black);

        if (isConnecting)
        {
            window.draw(currentConnection);
        }

        for (auto connection : Connection::connections)
        {
            connection->drawTo(window);
        }

        components.drawTo(window);

        for (auto gate : Gate::gates)
        {
            gate->logicOperation();
            gate->drawTo(window);
        }

        for (auto signal : OutputSignal::signals)
        {
            signal->drawTo(window);
        }
        for (auto signal : InputSignal::signals)
        {
            signal->drawTo(window);
        }

        status.drawTo(window);

        window.display();
    }
}

sf::Vector2f getMousePos(sf::RenderWindow &window)
{
    return (static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
}

int main()
{
    int width = 700;
    int height = 500;
    std::string title = "Logic CKT Simulator";

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;

    sf::RenderWindow window(sf::VideoMode(width, height), title, sf::Style::Fullscreen, settings);
    window.setFramerateLimit(60);

    FontManager::loadAssets(window);

    mainLoop(window);

    std::cout << "exited successfully" << std::endl;

    return 0;
}