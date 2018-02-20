#pragma once
#include <vector>
#include <functional>

class VAnimation
{
private:
	int totalFrames = 0;
	int currentFrame = 0;

	float frameDelay = 0;
	float frameTime = 0;

	std::vector<int> frames;
	std::function<void()> onCompleteCallback;

	bool looping = false;
	bool reverse = false;

public:
	VAnimation(const std::vector<int>& Frames, float FramesPerSecond, bool Looping, bool Reverse) : frames(Frames)
	{
		if (FramesPerSecond > 0)
			frameDelay = 1 / FramesPerSecond;
		else
			frameDelay = 0;

		totalFrames = frames.size();
		looping = Looping;
		reverse = Reverse;
	}

	virtual ~VAnimation() 
	{
		frames.clear();
		frames.shrink_to_fit();
	};

	//Resets animation.
	void Reset(std::function<void()> onComplete = nullptr)
	{
		currentFrame = 0;
		frameTime = 0;
		onCompleteCallback = onComplete;
	}

	//Sets current frame in animation. If no animation is playing, sets the frame to display.
	void SetCurrentFrame(int NewFrame)
	{
		if (NewFrame < totalFrames)
			currentFrame = NewFrame;
	}

	//Gets the current frame of an animation.
	int GetCurrentFrame()
	{
		return frames[currentFrame];
	}

	//Total amount of frames for animation.
	int GetFrameCount()
	{
		return totalFrames;
	}

	//Set animation to loop or not.
	void SetLooping(bool Looping)
	{
		looping = Looping;
	}

	//Sets animation to reverse or not.
	void SetReverse(bool Reverse)
	{
		reverse = Reverse;
	}

	void Update(float dt)
	{
		if (frameDelay > 0)
		{
			frameTime += dt;

			if (frameTime >= frameDelay)
			{
				int amount = 0;
				if (!reverse)
				{
					amount = 1;
				}
				else
				{
					amount = -1;
				}

				if (looping)
				{
					currentFrame = (currentFrame + totalFrames + amount) % totalFrames;
				}
				else
				{
					if (onCompleteCallback != nullptr)
					{
						if ((reverse && currentFrame == 0) ||
							(!reverse && currentFrame == totalFrames - 1))
						{
							onCompleteCallback();
							onCompleteCallback = nullptr;
						}
					}

					currentFrame = currentFrame + amount < 0 ? 
						0 : currentFrame + amount >= totalFrames ? 
						totalFrames - 1 : currentFrame + amount;
				}

				frameTime = 0;
			}
		}
	}
};

