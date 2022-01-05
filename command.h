#ifndef COMMAND_H
#define COMMAND_H

#include "position.h"
#include "sensor.h"
#include <initializer_list>
#include <memory>
#include <vector>

class command {
public:
    // Funkcja modyfikuje przekazaną pozycję i zwraca true, jeśli żaden z
    // czujników nie wykrył zagrożenia, w przeciwnym wypadku zwraca false.
    virtual bool execute(Position &pos, const std::vector<std::unique_ptr<Sensor>> &sensors) const = 0;

    virtual std::unique_ptr<command> clone() const = 0;
};

class move_forward : public command {
public:
    bool execute(Position &pos, const std::vector<std::unique_ptr<Sensor>> &sensors) const override {
        coordinate_t x = pos.get_x();
        coordinate_t y = pos.get_y();
        switch (pos.get_direction()) {
            case Direction::NORTH:
                y++;
                break;
            case Direction::EAST:
                x++;
                break;
            case Direction::SOUTH:
                y--;
                break;
            case Direction::WEST:
                x--;
                break;
        }
        for (auto &&sensor : sensors) {
            if (!sensor->is_safe(x, y)) {
                return false;
            }
        }
        pos.move(x, y);
        return true;
    }

    std::unique_ptr<command> clone() const override {
        return std::make_unique<move_forward>(*this);
    };
};

class move_backward : public command {
public:
    bool execute(Position &pos, const std::vector<std::unique_ptr<Sensor>> &sensors) const override {
        coordinate_t x = pos.get_x();
        coordinate_t y = pos.get_y();
        switch (pos.get_direction()) {
            case Direction::NORTH:
                y--;
                break;
            case Direction::EAST:
                x--;
                break;
            case Direction::SOUTH:
                y++;
                break;
            case Direction::WEST:
                x++;
                break;
        }
        for (auto &&sensor : sensors) {
            if (!sensor->is_safe(x, y)) {
                return false;
            }
        }
        pos.move(x, y);
        return true;
    }

    std::unique_ptr<command> clone() const override {
        return std::make_unique<move_backward>(*this);
    };
};

class rotate_left : public command {
public:
    bool execute(Position &pos, [[maybe_unused]] const std::vector<std::unique_ptr<Sensor>> &sensors) const override {
        Direction direction = pos.get_direction();
        switch (direction) {
            case Direction::NORTH:
                direction = Direction::WEST;
                break;
            case Direction::EAST:
                direction = Direction::NORTH;
                break;
            case Direction::SOUTH:
                direction = Direction::EAST;
                break;
            case Direction::WEST:
                direction = Direction::SOUTH;
                break;
        }
        pos.rotate(direction);
        return true;
    }

    std::unique_ptr<command> clone() const override {
        return std::make_unique<rotate_left>(*this);
    };
};

class rotate_right : public command {
public:
    bool execute(Position &pos, [[maybe_unused]] const std::vector<std::unique_ptr<Sensor>> &sensors) const override {
        Direction direction = pos.get_direction();
        switch (direction) {
            case Direction::NORTH:
                direction = Direction::EAST;
                break;
            case Direction::EAST:
                direction = Direction::SOUTH;
                break;
            case Direction::SOUTH:
                direction = Direction::WEST;
                break;
            case Direction::WEST:
                direction = Direction::NORTH;
                break;
        }
        pos.rotate(direction);
        return true;
    }

    std::unique_ptr<command> clone() const override {
        return std::make_unique<rotate_right>(*this);
    };
};

class compose : public command {
    std::vector<std::shared_ptr<command>> commands;

public:
    template<typename T>
    compose(const std::initializer_list<T> list) {
        for (auto cmd : list) {
            commands.push_back(cmd.clone());
        }
    }

    bool execute(Position &pos, const std::vector<std::unique_ptr<Sensor>> &sensors) const override {
        for (const auto &command : commands) {
            if (!command->execute(pos, sensors)) {
                return false;
            }
        }
        return true;
    }

    std::unique_ptr<command> clone() const override {
        return std::make_unique<compose>(*this);
    };
};

#endif // COMMAND_H
