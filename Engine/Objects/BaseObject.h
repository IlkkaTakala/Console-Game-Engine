#pragma once

class Data
{
public:
	Data() { ReferenceCount = 0; bMarked = false; }
	virtual void DestroyObject() = 0;
protected:
	 virtual ~Data() {}
	 bool bMarked;
private:
	friend class GC;
	template <class T>
	friend class Ref;
	int ReferenceCount;
};

template <class T>
class Ref
{
	T* Pointer;
	Data* DataPtr;
public:
	Ref() { Pointer = nullptr; DataPtr = nullptr; }
	Ref(T* ptr) { Pointer = ptr; DataPtr = nullptr; DataPtr = dynamic_cast<Data*>(Pointer); if (DataPtr != nullptr) DataPtr->ReferenceCount++; }
	~Ref() { if (DataPtr != nullptr) DataPtr->ReferenceCount--; }
	Ref(const Ref& old) { Pointer = old.Pointer; DataPtr = old.DataPtr; if (DataPtr != nullptr) DataPtr->ReferenceCount++; }

	Ref& operator=(const Ref& old) { 
		if (this != &old) { 
			Pointer = old.Pointer; 
			//if (DataPtr != nullptr) DataPtr->ReferenceCount--;
			DataPtr = dynamic_cast<Data*>(Pointer); 
			if (DataPtr != nullptr) DataPtr->ReferenceCount++;
		} 
		return *this;
	}
	T* operator->() const { return Pointer; }
	operator T* () const { return Pointer; }

	bool operator==(const Ref& other) { return Pointer == other.Pointer; }
	bool operator==(const void* other) { return Pointer == other; }
	bool operator!=(const Ref& other) { return Pointer != other.Pointer; }
	bool operator!=(const void* other) { return Pointer != other; }
};

class Tickable
{
protected:
	virtual ~Tickable() {};
public:
	virtual void Tick(float Delta) = 0;
};

class BaseObject : public Data
{
public:
	BaseObject() : Data() {}
	//void operator delete(void* ptr);

	virtual void BeginPlay() = 0;
	virtual void DestroyObject() override;
protected:
	virtual ~BaseObject() {};

};