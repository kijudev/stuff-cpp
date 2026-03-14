struct Pos {
    float x;
    float y;

    void serialize(ctx) {
        ctx.serialize(x, "x");
        ctx.serialize(y, "y");
    }
}
