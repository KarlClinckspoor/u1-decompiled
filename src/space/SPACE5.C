/* opts: -ms -v -w -cs */
/*
 * SPACE.EXE abnormal (3DD6-3DED): abnormal program termination; a module of its own
 * (its DATA starts word aligned).
 */
extern PUTS(), _EXIT();

/* abnormal termination: print message and exit(3) */
abnormal()
{
    PUTS("\nAbnormal program termination\n");
    _EXIT(3);
}
