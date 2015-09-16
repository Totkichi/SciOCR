int preprocessing( cv::Mat & , cv::Mat & );
int saveShred( const std::vector<cv::Rect> &, const cv::Mat &, const char* );
int shredder( std::vector<cv::Rect> &, cv::Mat & );
int createName( std::string &, const std::string &, const char* );
int getScreen( cv::Mat & );

//~ enum NiblackVersion;
static void usage (char);
double calcLocalStats (cv::Mat &, cv::Mat &, cv::Mat &, int , int );
//~ void NiblackSauvolaWolfJolion (cv::Mat, cv::Mat, NiblackVersion, int, int, double, double);
	
bool fileExists(char*);