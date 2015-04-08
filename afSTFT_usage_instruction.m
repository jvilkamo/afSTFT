function afSTFT_usage_instruction
% Example script for applying the afSTFT library
% - The first part shows how to apply it in a "single shot" mode
% - The second part shows how to apply it in a continuous mode
%
% Juha Vilkamo
% e-mail: juha.vilkamo@aalto.fi
%

% Init parameters and make input data
inChannels=2;
outChannels=2;
N=40000;
inputSignal = zeros(N,inChannels);
inputSignal(1,:)=1;



%% USAGE EXAMPLE IN AN "EVERYTHING AT ONCE" FUNCTION
inputSignalTimeFrequency = afAnalyze(inputSignal); %gives the signal in form (bands,samples,channels)

% processing, here a low-pass filter
outputSignalTimeFrequency = inputSignalTimeFrequency;
outputSignalTimeFrequency(10:end,:,:)=0;
outputSignal = afSynthesize(outputSignalTimeFrequency);
figure(1);freqz(outputSignal(:,1))



%% USAGE EXAMPLE IN A LOOP PROCESSING FUNCTION

% init the filter bank (sticking with these options recommended, or be sure if you consider very different choices)
hopSize=128; 
normalized_band_center_frequencies = afSTFT(hopSize,inChannels,outChannels,'hybrid'); % optional parameters 'hybrid' and 'low_delay'
% Band center frequencies = normalized_band_center_frequencies * Fs/2

frameSize = hopSize*8; % decide freely the frame size as a multiplication of the hopSize. For DirAC etc., frame size of 2048 time domain samples is a good choice.
outputSignal=zeros(N,outChannels);
sample=1;
while sample < N-frameSize
    % collect input and forward transform 
    inFrameTimeDomain = inputSignal([sample:sample+frameSize-1],:);
    inFrame = afSTFT(inFrameTimeDomain); % gives inFrame(bands,samples,channels) in time-frequency domain
    
    % processing, here a band-pass filter
    outFrame = inFrame;
    outFrame(1:10,:,:)=0;
    outFrame(20:end,:,:)=0;
    
    % inverse transform and save output
    outputSignal([sample:sample+frameSize-1],:) = afSTFT(outFrame);
    sample = sample+frameSize;
end
 figure(2);freqz(outputSignal(:,1))


 afSTFT(); % Free
 