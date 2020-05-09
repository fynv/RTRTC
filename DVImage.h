#ifndef _DVImage_h
#define _DVImage_h

#include "RTRTC_api.h"
#include "DeviceViewable.h"

class RT_RTC_API DVImageLike : public DeviceViewable
{
public:
	std::string name_elem_cls() const { return m_elem_cls; }
	std::string name_ref_type() const { return m_ref_type; }
	int elem_size() const { return m_elem_size; }
	int width() const { return m_width;  }
	int height() const { return m_height;  }

	DVImageLike(const char* elem_cls, const char* ref_type, int width, int height);
	virtual ~DVImageLike() {}
	virtual bool is_readable() const { return true; }
	virtual bool is_writable() const { return false; }

protected:
	std::string m_elem_cls;
	std::string m_ref_type;
	int m_elem_size;
	int m_width;
	int m_height;
};

class RT_RTC_API DVImage : public DVImageLike
{
public:
	void* data() const { return m_data; }

	DVImage(const char* elem_cls, int width, int height, void* hdata = nullptr);
	~DVImage();
	virtual bool is_writable() const { return true; }

	void to_host(void* hdata) const;
	virtual ViewBuf view() const;

private:
	void* m_data;
};


#endif


