// Kody Manastyrski
// Axis.hpp
//


#ifndef AXIS
#define AXIS

class Axis{

	public:

		enum Type {linear, log};
		Axis(enum Type t){scale_step = 0; type = t; min = 0; max = 25; };

		double getMinVal();
		double getMaxVal();
		double getRange();

		double getMajorTickStep();
		double getMinorTickStep();

		enum Type getType();
		void setType(enum Type t);

	private:
		int scale_step;
		double min, max;
		Type type;

	protected:

};

#endif
