
#include <iostream>
#include <gtest/gtest.h>

#include <converter/Converter.hpp>
#include <converter/Convertible.hpp>


TEST(XML_TO_AST_TESTS, SimplestWayPossible) {
    std::string text = R"(<hello>world</hello>)";

    AST ast = XMLConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << XMLConverter::from_ast(ast) << std::endl;

    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << XMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(XML_TO_AST_TESTS, PrettyHardTest) {
    std::string text = R"(<root><array><of>objects</of><you-know>12</you-know></array><object><end><hello>world</hello><wow><even><deeper><hello>world</hello></deeper></even></wow></end><hello>world</hello></object></root>)";

    AST ast = XMLConverter::to_ast(text);

    std::cout << ast << std::endl;
    std::cout << XMLConverter::from_ast(ast) << std::endl;


    AST good_ast = std::make_shared<ASTNode>(ASTNode::Object{
        {"root", std::make_shared<ASTNode>(ASTNode::Object{
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
            {"array", std::make_shared<ASTNode>(ASTNode::Object{
                {"of", std::make_shared<ASTNode>("objects")},
                {"you-know", std::make_shared<ASTNode>("12")}
            })}
        }) 
        }  
    });


    std::cout << XMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(ast, good_ast);
}

TEST(XML_FROM_AST_TESTS, SimplestWayPossible) {
    AST ast = std::make_shared<ASTNode>(ASTNode::Object{{"hello", std::make_shared<ASTNode>("world")}});
    std::cout << XMLConverter::from_ast(ast) << std::endl;

    EXPECT_EQ(XMLConverter::from_ast(ast), R"(<hello>world</hello>)");
}

TEST(XML_FROM_AST_TESTS, PrettyHardTest) {
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

    std::cout << XMLConverter::from_ast(good_ast) << std::endl;

    EXPECT_EQ(XMLConverter::from_ast(good_ast), R"(<array><of>objects</of><you-know>12</you-know></array><object><end><hello>world</hello><wow><even><deeper><hello>world</hello></deeper></even></wow></end><hello>world</hello></object>)");
}
