class Landmark
{
public:
	Landmark(string name)
	{
		m_name = name;
	}
	virtual string color() const = 0;
	virtual string icon() const = 0;
	string name() const { return m_name; }
	virtual ~Landmark() {}
private:
	string m_name;
};

class Hotel : public Landmark
{
public:
	Hotel(string name) : Landmark(name)
	{
		m_name = name;
	}
	virtual string color() const
	{
		return "yellow";
	}
	virtual string icon() const
	{
		return "bed";
	}
	virtual ~Hotel()
	{
		cout << "Destroying the hotel " << name() << "." << endl;
	}
private:
	string m_name;
};

class Restaurant : public Landmark
{
public:
	Restaurant(string name, int capacity) : Landmark(name)
	{
		m_name = name;
		m_capacity = capacity;
	}
	virtual string color() const
	{
		return "yellow";
	}
	virtual string icon() const
	{
		if (m_capacity < 40)
			return "small knife/fork";
		return "large knife/fork";
	}
	virtual ~Restaurant()
	{
		cout << "Destroying the restaurant " << name() << "." << endl;
	}
private:
	string m_name;
	int m_capacity;
};

class Hospital : public Landmark
{
public:
	Hospital(string name) : Landmark(name)
	{
		m_name = name;
	}
	virtual string color() const
	{
		return "blue";
	}
	virtual string icon() const
	{
		return "H";
	}
	virtual ~Hospital()
	{
		cout << "Destroying the hospital " << name() << "." << endl;
	}
private:
	string m_name;
};
