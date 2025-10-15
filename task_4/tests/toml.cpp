
#include <iostream>
#include <gtest/gtest.h>

#include <converter/Converter.hpp>
#include <converter/Convertible.hpp>


TEST(TOML_TO_AST_TESTS, SimplestWayPossible) {
    std::string text = R"(
    hello = "world"
)";

    AST ast = TOMLConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << TOMLConverter::from_ast(ast) << std::endl;

    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << TOMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(TOML_TO_AST_TESTS, PrettyHardTest) {
    std::string text = R"(
    array = [{of = "objects", you-know = 12}]

    [object]
    hello = "world"

    [object.end]
    hello = "world"

    [object.end.wow]

    [object.end.wow.even]

    [object.end.wow.even.deeper]
    hello = "world"
)";

    AST ast = TOMLConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << TOMLConverter::from_ast(ast) << std::endl;


    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{
        {"object", std::make_shared<ASTNode>(ASTNode::Object{
            {"hello", std::make_shared<ASTNode>("world")},
            {"end", std::make_shared<ASTNode>(ASTNode::Object{
                {"hello", std::make_shared<ASTNode>("world")},
                {"wow", std::make_shared<ASTNode>(ASTNode::Object{
                    {"even", std::make_shared<ASTNode>(ASTNode::Object{
                        {"deeper", std::make_shared<ASTNode>(ASTNode::Object{
                            {"hello", std::make_shared<ASTNode>("world")}
                        })}
                    })}
                })}
            })}
        })},
        {"array", std::make_shared<ASTNode>(ASTNode::Array{
            std::make_shared<ASTNode>(ASTNode::Object{
                {"of", std::make_shared<ASTNode>("objects")},
                {"you-know", std::make_shared<ASTNode>(12.0)}
            })
        })}
    });

    std::cout << TOMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(TOML_FROM_AST_TESTS, SimplestWayPossible) {
    AST ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << TOMLConverter::from_ast(ast) << std::endl;

    EXPECT_EQ(TOMLConverter::from_ast(ast), R"(hello = "world"
)");
}

TEST(TOML_FROM_AST_TESTS, PrettyHardTest) {
    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{
        {"object", std::make_shared<ASTNode>(ASTNode::Object{
            {"hello", std::make_shared<ASTNode>("world")},
            {"end", std::make_shared<ASTNode>(ASTNode::Object{
                {"hello", std::make_shared<ASTNode>("world")},
                {"wow", std::make_shared<ASTNode>(ASTNode::Object{
                    {"even", std::make_shared<ASTNode>(ASTNode::Object{
                        {"deeper", std::make_shared<ASTNode>(ASTNode::Object{
                            {"hello", std::make_shared<ASTNode>("world")}
                        })}
                    })}
                })}
            })}
        })},
        {"array", std::make_shared<ASTNode>(ASTNode::Array{
            std::make_shared<ASTNode>(ASTNode::Object{
                {"of", std::make_shared<ASTNode>("objects")},
                {"you know", std::make_shared<ASTNode>(12.0)}
            })
        })}
    });

    std::cout << TOMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(TOMLConverter::from_ast(good_ast), R"(array = [{of = "objects", you know = 12}]

[object]
hello = "world"

[object.end]
hello = "world"

[object.end.wow]

[object.end.wow.even]

[object.end.wow.even.deeper]
hello = "world"
)");
}
