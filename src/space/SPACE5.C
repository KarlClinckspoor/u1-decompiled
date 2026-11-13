/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE F_3DD6 (3DD6-3DED): abnormal program termination; a module of its own
 * (its DATA starts word aligned).
 */
extern PUTS(), _EXIT();

/* abnormal termination: print message and exit(3) */
F_3DD6()
{
    PUTS("\nAbnormal program termination\n");
    _EXIT(3);
}
