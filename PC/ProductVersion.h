/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// ProductVersion reads on construction the version-information from a file into a buffer 

class ProductVersion
{ 
public:

    /**
     * Create an instance of a version object, read version-resource
     * from module filePath, if filePath is epmty use own module
     */
    ProductVersion(LPCTSTR  pFilePath = NULL);
    
    ~ProductVersion();

    /** 
      * Get the version information from the current module 
      * return a pointer to the VERSIONINFO of this module 
      */
    const void *GetVersionOfModule();

    /** 
      * Read product version string from version info and return it
      * in parameter productVersion
      */
    void GetProductVersionString(CString& productVersion);

    /** 
      * Read product name string from version info and return it
      * in parameter productVersion
      */
    void GetProductNameString(CString& productName);

    /** return the binary version-information of a file */
    void GetFileVersionNumber(DWORD& dwVersionNumber, DWORD& dwBuildNumber);
    
private:
    void* m_pVerInfoBuf;
    DWORD m_dwFileInfoSize;
};
