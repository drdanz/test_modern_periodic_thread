/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

class Tracer
{
public:
    Tracer() {
        std::cout << "Tracer default constructor called" << std::endl;
    }

    ~Tracer() {
        std::cout << "Tracer destructor called" << std::endl;
    }

    Tracer(const Tracer &) {
        std::cout << "Tracer copy constructor called" << std::endl;
    }

    Tracer(Tracer &&) noexcept {
        std::cout << "Tracer move constructor called" << std::endl;
    }

    Tracer& operator=(const Tracer &) {
        std::cout << "Tracer copy assignment operator called" << std::endl;
        return *this;
    }

    Tracer& operator=(Tracer &&) noexcept {
        std::cout << "Tracer move assignment operator called" << std::endl;
        return *this;
    }
};

