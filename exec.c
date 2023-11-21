

int exec(const char* path, void* loadAddress, unsigned* entryPointPtr)
{
    unsigned end = pentry.address + amountToLoad;

    //do this whenever you do file read fully in exec
    if( pentry.address < 0x400000 ||
        end > 0x800000 ||
        end < pentry.address ){
            rv = ENOEXEC;
            goto error;
    }
    rv = file_read_fully(fd,
        (void*)(pentry.address+delta),
        amountToLoad);
    //


    //at bottom remove assembly before it
    *entryPointPtr = pentry.entry;
    return SUCCESS;
}

