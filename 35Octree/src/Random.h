#pragma once

#include <MathCore/MathCore.h>
#include <random>

namespace Utils{

	template <typename _int_type_>
	struct RandomDefinition{
	};

	template <>
	struct RandomDefinition<uint32_t>{
		using mt19937 = std::mt19937;
		static __forceinline uint32_t randomSeed() noexcept{

			return std::random_device{}();
		}
	};

	template <>
	struct RandomDefinition<uint64_t>{
		using mt19937 = std::mt19937_64;
		static __forceinline uint64_t randomSeed() noexcept{

			std::random_device rnd_sd;
			uint64_t rnd_sd_64 = rnd_sd();
			rnd_sd_64 = (rnd_sd_64 << 32) | (uint64_t)rnd_sd();
			return rnd_sd_64;
		}
	};

	template <typename _type_>
	class RandomTemplate{
		using self_type = RandomTemplate<_type_>;
		using random_definition = RandomDefinition<_type_>;
		using mt19937_type = typename random_definition::mt19937;

		mt19937_type mt;

	public:
		_type_ seed;

		RandomTemplate(_type_ seed = mt19937_type::default_seed){

			this->setSeed(seed);
		}

		void setSeed(_type_ s){
			seed = s;
			mt.seed(s);
			mt.discard(700000);
		}

		_type_ randomSeed(){
			_type_ rnd_sd = random_definition::randomSeed();
			this->setSeed(rnd_sd);
			return rnd_sd;
		}

		double getDouble01(){
			std::uniform_real_distribution<double> dist(0.0, 1.0);
			return dist(mt);
		}

		float getFloat01(){
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);
			return dist(mt);
		}

		template <typename f_type,
			typename std::enable_if<
			std::is_same<f_type, float>::value,
			bool>::type = true>
			f_type next01()
		{
			return getFloat01();
		}

		template <typename f_type,
			typename std::enable_if<
			std::is_same<f_type, double>::value,
			bool>::type = true>
			f_type next01()
		{
			return getDouble01();
		}

		int getRange(int min, int max){
			std::uniform_int_distribution<int> dist(min, max);
			return dist(mt);
		}

		static self_type *Instance(){
			static self_type _rnd(random_definition::randomSeed());
			return &_rnd;
		}
	};

	using Random32 = Utils::RandomTemplate<uint32_t>;
	using Random64 = Utils::RandomTemplate<uint64_t>;
	using Random = Random32;

	template <typename TRandom>
	class MathRandomExt
	{
	public:
		TRandom *random;

		MathRandomExt(TRandom *_rnd_)
		{
			random = _rnd_;
		}

		template <typename f_type,
			typename std::enable_if<
			std::is_same<f_type, float>::value,
			bool>::type = true>
			f_type next01()
		{
			return random->getFloat01();
		}

		template <typename f_type,
			typename std::enable_if<
			std::is_same<f_type, double>::value,
			bool>::type = true>
			f_type next01()
		{
			return random->getDouble01();
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value,
			bool>::type = true>
			__forceinline _math_type nextRange(const _math_type &min, const _math_type &max)
		{
			_math_type delta = max - min;
			delta *= next<_math_type>();
			return min + delta;
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_integral<typename  MathCore::MathTypeInfo<_math_type>::_type>::value &&
			MathCore::MathTypeInfo<_math_type>::_is_vec::value,
			bool>::type = true>
			__forceinline _math_type nextRange(const _math_type &min, const _math_type &max)
		{
			_math_type result = _math_type();
			for (int i = 0; i < (int)_math_type::array_count; i++)
				result[i] = random->getRange(min[i], max[i]);
			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_integral<typename MathCore::MathTypeInfo<_math_type>::_type>::value &&
			!MathCore::MathTypeInfo<_math_type>::_is_vec::value,
			bool>::type = true>
			__forceinline _math_type nextRange(const _math_type &min, const _math_type &max)
		{
			_math_type result = _math_type();
			for (int c = 0; c < (int)_math_type::cols; c++)
				for (int r = 0; r < (int)_math_type::rows; r++)
					result[c][r] = random->getRange(min[c][r], max[c][r]);

			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value &&
			MathCore::MathTypeInfo<_math_type>::_is_vec::value,
			bool>::type = true>
			__forceinline _math_type next()
		{
			using _f_type = typename MathCore::MathTypeInfo<_math_type>::_type;

			_math_type result = _math_type();
			for (int i = 0; i < (int)_math_type::array_count; i++)
				result[i] = next01<_f_type>();
			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value &&
			!MathCore::MathTypeInfo<_math_type>::_is_vec::value,
			bool>::type = true>
			__forceinline _math_type next()
		{
			using _f_type = typename MathCore::MathTypeInfo<_math_type>::_type;

			_math_type result = _math_type();
			for (int c = 0; c < (int)_math_type::cols; c++)
				for (int r = 0; r < (int)_math_type::rows; r++)
					result[c][r] = next01<_f_type>();
			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value &&
			MathCore::MathTypeInfo<_math_type>::_is_vec::value,
			bool>::type = true>
			__forceinline _math_type nextDirection(bool homogeneous = false)
		{
			using _f_type = typename MathCore::MathTypeInfo<_math_type>::_type;
			_math_type result = _math_type();
			do
			{
				for (int i = 0; i < (int)_math_type::array_count - (int)homogeneous; i++)
					result[i] = next01<_f_type>();
				result = result * (_f_type)2 - (_f_type)1;
				if (homogeneous)
					result[_math_type::array_count - 1] = (_f_type)0;
				result = OP<_math_type>::normalize(result);
			} while (OP<_math_type>::sqrLength(result) < 0.25);
			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			(std::is_same<typename MathCore::MathTypeInfo<_math_type>::_class, MathCore::MathTypeClass::_class_vec3>::value ||
				std::is_same<typename MathCore::MathTypeInfo<_math_type>::_class, MathCore::MathTypeClass::_class_vec4>::value) && 
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value,
			bool>::type = true>
			__forceinline _math_type nextUVW()
		{
			using _f_type = typename MathCore::MathTypeInfo<_math_type>::_type;

			_f_type u = next01<_f_type>();
			_f_type v = next01<_f_type>();
			_f_type w = (_f_type)1 - u - v;
			if (w < (_f_type)0)
			{
				u = (_f_type)1 - u;
				v = (_f_type)1 - v;
				w = -w;
			}
			_math_type result;
			result[0] = u;
			result[1] = v;
			result[2] = w;
			return result;
		}

		template <typename _math_type,
			typename std::enable_if<
			(std::is_same<typename MathCore::MathTypeInfo<_math_type>::_class, MathCore::MathTypeClass::_class_vec3>::value ||
				std::is_same<typename MathCore::MathTypeInfo<_math_type>::_class, MathCore::MathTypeClass::_class_vec4>::value) &&
			std::is_floating_point<typename MathCore::MathTypeInfo<_math_type>::_type>::value,
			bool>::type = true>
			__forceinline _math_type nextPointInsideTriangle(const _math_type &a, const _math_type &b, const _math_type &c)
		{
			using _f_type = typename MathCore::MathTypeInfo<_math_type>::_type;

			_f_type u = next01<_f_type>();
			_f_type v = next01<_f_type>();
			_f_type w = (_f_type)1 - u - v;
			if (w < (_f_type)0)
			{
				u = (_f_type)1 - u;
				v = (_f_type)1 - v;
				w = -w;
			}
			_math_type result = a * u + b * v + c * w;
			if (std::is_same<typename MathCore::MathTypeInfo<_math_type>::_class, MathCore::MathTypeClass::_class_vec4>::value)
				result[3] = (_f_type)1;
			return result;
		}

	};
}
