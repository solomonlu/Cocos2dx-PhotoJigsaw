#ifndef __FRAME_PHOTOMANAGER_H__
#define __FRAME_PHOTOMANAGER_H__

#include <vector>
#include <string>

class PhotoManager
{
	typedef std::vector<std::string> PhotoFileNameList;

	typedef struct tagPhotoFragmentRecord
	{
		std::string photoFileName;
		unsigned int FragmentMask;		// 每位都标识碎片是否得到（0:没得到，1:已得到）
	}PhotoFragmentRecord;
	typedef std::vector<PhotoFragmentRecord> PhotoFragmentRecordList;
public:
	static PhotoManager* getInstance();

	// 照片相关函数
	std::vector<PhotoFileNameList>& getAllPhotos(){return _photos;}

	// 碎片相关函数
	PhotoFragmentRecordList& getAllPhotoFragments(){return _photoFragments;}
	unsigned int getFragmentMask(const std::string& photoFileName);
	void addFragmentMask(const std::string& photoFileName,unsigned int newFragmentMask);
	void delFragment(const std::string& photoFileName);
private:
	void init();

	void photoFragmentReadFromFile();
	void photoFragmentFlushToFile(const PhotoFragmentRecord& photoFragments);

	static PhotoManager* _instance;

	std::vector<PhotoFileNameList> _photos;
	PhotoFragmentRecordList _photoFragments;
};

#endif // __FRAME_PHOTOMANAGER_H__