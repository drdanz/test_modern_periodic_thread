enum FlagsA
{
    AA = 1 << 0, // binary 0001
    AB = 1 << 1, // binary 0010
    AC = 1 << 2, // binary 0100
    AD = 1 << 3, // binary 1000
};

enum FlagsB
{
    BA = 1 << 0, // binary 0001
    BB = 1 << 1, // binary 0010
    BC = 1 << 2, // binary 0100
    BD = 1 << 3, // binary 1000
};

inline FlagsA operator|(FlagsA l, FlagsA r)
{
    return static_cast<FlagsA>(l | r);
}

int main()
{
    FlagsA f = AA | AB;

    int i = f +4;
    bool b = AA && BB;
}
