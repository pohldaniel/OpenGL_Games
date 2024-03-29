#include <vector>
#include <engine/Vector.h>

namespace Utils
{
	template <class C>
	class Key
	{
	public:
		float time;
		C value;
		Key() {}
		Key(float _time, const C &_value)
		{
			time = _time;
			value = _value;
		}
	};

	template <class T>
	class LinearInterpolator
	{

		float lastTimeQuery;
		int lastIndexA;
		int lastIndexB;
		T lastReturned;

	public:
		typename std::vector<Key<T>> keys;

		LinearInterpolator()
		{
			lastTimeQuery = 0.0f;
			lastIndexA = 0;
			lastIndexB = 1;
			lastReturned = T();
		}

		void addKey(const Key<T> &v)
		{
			keys.push_back(v);
			if (keys.size() == 1)
			{
				lastTimeQuery = v.time;
				lastReturned = v.value;
			}
		}

		void resetToBegin()
		{
			if (keys.size() > 0)
			{
				lastIndexA = 0;
				lastIndexB = 1;
				lastTimeQuery = keys[0].time;
				lastReturned = keys[0].value;
			}
		}

		T getValue_ForwardLoop(float t, T *interquery_delta = NULL)
		{
			if (keys.size() == 0)
				return T();
			else if (keys.size() == 1)
				return keys[0].value;

			t = MathCore::OP<float>::clamp(t, keys[0].time, keys[keys.size() - 1].time);

			if (t < lastTimeQuery)
			{
				// add the last animation value related to the last returned
				if (interquery_delta != NULL)
					*interquery_delta = keys[keys.size() - 1].value - lastReturned;
				resetToBegin();
			}
			else
			{
				if (interquery_delta != NULL)
					*interquery_delta = T(0);
			}

			if (t > lastTimeQuery)
			{
				lastTimeQuery = t;

				for (; lastIndexA < keys.size() - 2; lastIndexA++)
				{
					if (t < keys[lastIndexA + 1].time)
						break;
				}
				lastIndexB = lastIndexA + 1;
			}
			else
			{
				// if (interquery_delta != NULL)
				//     *interquery_delta = T(0);
				return lastReturned;
			}

			float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
			float lrp;

			if (delta <= MathCore::EPSILON<float>::high_precision)
				lrp = 0.0f;
			else
				lrp = (t - keys[lastIndexA].time) / delta;

			T result = MathCore::OP<T>::lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

			if (interquery_delta != NULL)
				*interquery_delta += result - lastReturned;

			lastReturned = result;
			return lastReturned;
		}

		// binary search
		T getValue(float t)
		{
			if (keys.size() == 0)
				return T();
			else if (keys.size() == 1)
				return keys[0].value;

			//t = MathCore::OP<float>::clamp(t, keys[0].time, keys[keys.size() - 1].time);
			t = Math::Clamp(t, keys[0].time, keys[keys.size() - 1].time);

			if (lastTimeQuery == t)
				return lastReturned;
			else if (t <= keys[0].time)
			{
				lastIndexA = 0;
				lastIndexB = 1;
				lastTimeQuery = t;
				lastReturned = keys[lastIndexA].value;
				return lastReturned;
			}
			else if (t >= keys[keys.size() - 1].time)
			{
				lastIndexB = keys.size() - 1;
				lastIndexA = lastIndexB - 1;
				lastTimeQuery = t;
				lastReturned = keys[lastIndexB].value;
				return lastReturned;
			}

			// binary search
			int start_index = 0;
			int end_index = keys.size() - 1;
			int middle_index = (start_index + end_index) >> 1;

			float t_start = keys[start_index].time;
			float t_end = keys[end_index].time;
			float t_middle = keys[middle_index].time;
			while (end_index - start_index > 1)
			{
				if (t > t_middle)
				{
					start_index = middle_index;
					t_start = t_middle;

					middle_index = (start_index + end_index) >> 1;
					t_middle = keys[middle_index].time;
				}
				else if (t < t_middle)
				{
					end_index = middle_index;
					t_end = t_middle;

					middle_index = (start_index + end_index) >> 1;
					t_middle = keys[middle_index].time;
				}
				else
				{
					// the middle is the value we want...
					lastIndexA = middle_index - 1;
					lastIndexB = middle_index;
					if (lastIndexA < 0)
					{
						lastIndexA = middle_index;
						lastIndexB = middle_index + 1;
					}
					lastTimeQuery = t;
					lastReturned = keys[middle_index].value;
					return lastReturned;
				}
			}

			lastTimeQuery = t;
			lastIndexA = start_index;
			lastIndexB = end_index;

			float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
			float lrp;

			if (delta <= MathCore::EPSILON2<float>::high_precision)
				lrp = 0.0f;
			else
				lrp = (t - keys[lastIndexA].time) / delta;

			//lastReturned = MathCore::OP<T>::lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);
			lastReturned = Math::Lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);
			return lastReturned;
		}

		T getValue_WalkStep(float t)
		{
			if (keys.size() == 0)
				return T();
			else if (keys.size() == 1)
				return keys[0].value;

			t = MathCore::OP<float>::clamp(t, keys[0].time, keys[keys.size() - 1].time);

			if (t > lastTimeQuery)
			{
				lastTimeQuery = t;

				for (; lastIndexA < keys.size() - 2; lastIndexA++)
				{
					if (t < keys[lastIndexA + 1].time)
						break;
				}
				lastIndexB = lastIndexA + 1;
			}
			else if (t < lastTimeQuery)
			{
				lastTimeQuery = t;

				for (; lastIndexB > 1; lastIndexB--)
				{
					if (t > keys[lastIndexB - 1].time)
						break;
				}
				lastIndexA = lastIndexB - 1;
			}
			else
				return lastReturned;

			float delta = (keys[lastIndexB].time - keys[lastIndexA].time);
			float lrp;

			if (delta <= MathCore::EPSILON<float>::high_precision)
				lrp = 0.0f;
			else
				lrp = (t - keys[lastIndexA].time) / delta;

			lastReturned = MathCore::OP<T>::lerp(keys[lastIndexA].value, keys[lastIndexB].value, lrp);

			return lastReturned;
		}

		T getValue_slow(float t) const
		{

			if (keys.size() == 0)
				return T();
			else if (keys.size() == 1)
				return keys[0].value;

			t = MathCore::OP<float>::clamp(t, keys[0].time, keys[keys.size() - 1].time);

			int indexA;

			for (indexA = 0; indexA < keys.size() - 1; indexA++)
			{
				if (t < keys[indexA + 1].time)
					break;
			}

			if (indexA >= keys.size() - 1)
				return keys[keys.size() - 1].value;

			int indexB = indexA + 1;

			float delta = (keys[indexB].time - keys[indexA].time);
			float lrp;

			if (delta <= MathCore::EPSILON<float>::high_precision)
				lrp = 0.0f;
			else
				lrp = (t - keys[indexA].time) / delta;

			return MathCore::OP<T>::lerp(keys[indexA].value, keys[indexB].value, lrp);
		}
	};
}