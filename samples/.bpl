make [S]name: "So..."{END{
    Wrap{
        quotes[4]::[TERMINATE]
    }
    Reform{
        alloc: 64
        PushValue{
            To: START
            Value: "Hello!\n"
            To: END
            Value: "I added in some new stuff :D"
        }
    }
    reference: n
}};
make [S]namE: brand namE{
    memalloc
};
make [string] name: "AIDAN";
print[any](name,n,namE);