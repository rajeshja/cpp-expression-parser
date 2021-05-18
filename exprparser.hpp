class ExpressionParser {
    public:
    ExpressionParser();
    ExpressionParser(const char* expression);
    void dump();
    private:
    const char* expression;
};
