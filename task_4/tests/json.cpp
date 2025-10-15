#include <iostream>
#include <gtest/gtest.h>

#include <converter/Converter.hpp>
#include <converter/Convertible.hpp>


TEST(JSON_TO_AST_TESTS, SimplestWayPossible) {
    std::string text = R"({
        "hello": "world"
    })";

    AST ast = JSONConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << JSONConverter::from_ast(ast) << std::endl;

    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << JSONConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(JSON_TO_AST_TESTS, PrettyHardTest) {
    std::string text = R"({
        "object": {
            "hello": "world",
            "end": {
                "hello": "world",
                "wow": {
                    "even": {
                        "deeper": {
                            "hello": "world"
                        }
                    }
                }
            }
        },
        "array": [
            {
                "of": "objects",
                "you know": 12
            }
        ]
    })";

    AST ast = JSONConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << JSONConverter::from_ast(ast) << std::endl;


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

    std::cout << JSONConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(JSON_FROM_AST_TESTS, SimplestWayPossible) {
    AST ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << JSONConverter::from_ast(ast) << std::endl;

    EXPECT_EQ(JSONConverter::from_ast(ast), R"({"hello":"world"})");
}

TEST(JSON_FROM_AST_TESTS, PrettyHardTest) {
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

    std::cout << JSONConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(JSONConverter::from_ast(good_ast), R"({"array":[{"of":"objects","you know":12}],"object":{"end":{"hello":"world","wow":{"even":{"deeper":{"hello":"world"}}}},"hello":"world"}})");
}
