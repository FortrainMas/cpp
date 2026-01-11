#include <iostream>
#include <memory>

struct IDrawableConcept {
    virtual void draw() = 0;
    virtual ~IDrawableConcept() = default;
};

struct Square : IDrawableConcept {
    void draw() override { std::cout << "Draw a square" << std::endl; }
};

struct Circle : IDrawableConcept {
    void draw() override { std::cout << "Draw a circle" << std::endl; }
};

class IDrawable {
    private:
        std::unique_ptr<IDrawableConcept> drawable;
    public:
        template<class T>
        IDrawable(T drawable) : drawable(std::make_unique<T>(drawable)) {};
        void draw() { drawable->draw(); }
};


int main () {
    IDrawable square = Square{};
    IDrawable circle = Circle{};
    square.draw();
    circle.draw();
}
