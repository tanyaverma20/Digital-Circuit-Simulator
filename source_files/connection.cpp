#include "../header_files/connection.hpp"

std::vector<Connection *> Connection::connections;

Connection::Connection(InputPort *ip, OutputPort *op)
{
    this->ip = ip;
    this->op = op;
    connect();
}

void Connection::connect()
{
    ip->isConnected = true;
    ip->parentPort = op;
    op->childPorts.push_back(ip);
    updateState();
    drawConnection();
}

Connection::~Connection()
{
    std::cout << "deleted connection at " << this << std::endl;
    delete this;
}

void Connection::drawConnection()
{
    sf::Vector2f startPos = op->getPosition();
    sf::Vector2f endPos = ip->getPosition();
    sf::Vector2f vec = endPos - startPos;
    float mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    float angle = (180.f / 3.141592f) * std::atan2(vec.y, vec.x);

    connection.setPosition(startPos);
    connection.setSize(sf::Vector2f(mag, 3.f));
    sf::FloatRect bounds = connection.getLocalBounds();
    connection.setOrigin(0, bounds.height / 2);
    connection.setRotation(angle);
}

void Connection::drawTo(sf::RenderWindow &window)
{
    drawConnection();
    updateState();
    window.draw(connection);
}

void Connection::updateState()
{
    ip->setState(op->getState());
}
