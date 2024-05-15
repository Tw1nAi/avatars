// R&D on audio capture and audio streaming via websockets

// if (WhisperWebsocket == nullptr || !WhisperWebsocket->IsConnected()) return;

// Prepare a TArray<int16> for the converted audio data
// TArray<int16> Int16AudioData;
// Int16AudioData.Reserve(Data.Num());

// // Convert each float sample to int16
// for (float Sample : Data)
// {
//   // Clamp the value to the range [-1.0, 1.0], then scale and convert
//   int16 ConvertedSample = FMath::Clamp(Sample, -1.0f, 1.0f) * 32767.0f;
//   Int16AudioData.Add(ConvertedSample);
// }

// // Convert int16 array to byte array
// TArray<uint8> ByteArray;
// ByteArray.Reserve(Int16AudioData.Num() * sizeof(int16));

// // Copy the int16 audio data to the byte array
// for (int16 Sample : Int16AudioData)
// {
//   // Split the int16 into two bytes and add them to the array
//   ByteArray.Add((Sample >> 8) & 0xFF); // High byte
//   ByteArray.Add(Sample & 0xFF);        // Low byte
// }

// Example TArray<int16> PcmData; filled elsewhere
// TArray<float> NormalizedAudioData;
// NormalizedAudioData.Reserve(PcmData.Num());

// for (int16 Sample : PcmData)
// {
//   // Convert and normalize the int16 sample to float
//   float NormalizedSample = FMath::Clamp(static_cast<float>(Sample) / 32768.0f, -1.0f, 1.0f);
//   NormalizedAudioData.Add(NormalizedSample);
// }

// WhisperWebsocket->Send(ByteArray.GetData(), ByteArray.Num(), true);

// TArray<float> AudioWithGainData;
// AudioWithGainData.Reserve(PcmData.Num());
// for (float Sample : PcmData)
// {
//   AudioWithGainData.Add(Sample * InputGain);
// }

// TArray<int16> Int16AudioData;
// for (float Sample : PcmData)
// {
//   // Convert float sample [-1.0, 1.0] to int16
//   int16 ConvertedSample = FMath::Clamp(Sample * 32767.0f, -32768.0f, 32767.0f);
//   Int16AudioData.Add(ConvertedSample);
// }

// // Optionally convert Int16Array to ByteArray if your websocket API requires it
// TArray<uint8> ByteArray;
// ByteArray.Reserve(Int16AudioData.Num() * sizeof(int16));
// for (int16 Sample : Int16AudioData)
// {
//   // Split int16 to two bytes and store
//   ByteArray.Add((Sample >> 8) & 0xFF);
//   ByteArray.Add(Sample & 0xFF);
// }

// TArray<uint8> ByteArray(reinterpret_cast<uint8*>(ModifiedData.GetData()), ModifiedData.Num() * sizeof(float));
// TArray<int16> ByteArray(reinterpret_cast<int16*>(ModifiedData.GetData()), ModifiedData.Num() * sizeof(float));
// URuntimeAudioTranscoder::TranscodeRAWDataFromBuffer(
//     ModifiedData,
//     ERuntimeRAWAudioFormat::Float32,
//     ERuntimeRAWAudioFormat::Int16,
//     TranscodedDataEvent);

// TArray<float> ResampledAudio = ResampleAudioData(AudioWithGainData, AudioWithGainData.Num(), 1024);

// SIZE_T SizeInBytes = PcmData.Num() * sizeof(float);
// WhisperWebsocket->Send(PcmData.GetData(), SizeInBytes, true);
// WhisperWebsocket->Send(Int16AudioData.GetData(), Int16AudioData.Num(), true);

// / if (RuntimeAudioExporter != nullptr && CapturableSoundWave != nullptr)
// {
//   FOnAudioExportToBufferResultNative AudioExportResult;
//   AudioExportResult.BindUObject(this, &AAvatarsPlayerController::OnAudioBytesExported);

//   TWeakObjectPtr<UCapturableSoundWave> SoundWavePtr = CapturableSoundWave;

//   RuntimeAudioExporter->ExportSoundWaveToRAWBuffer(
//       SoundWavePtr,
//       RuntimeRAWAudioFormat,
//       OverrideOptions,
//       AudioExportResult);

//   FOnPlayedAudioDataReleaseResultNative Result;
//   CapturableSoundWave->ReleasePlayedAudioData(Result);
// }

// TArray<uint8> ConvertedAudioData;
// for (float Sample : Data)
// {
//   // Convert and format the audio data here
//   // You may need to convert it to the appropriate format (e.g., float32) and format as bytes
//   float ConvertedSample = static_cast<float>((int16)Sample) / 32768.0f;
//   ConvertedAudioData.Add(static_cast<uint8>(ConvertedSample * 255));
// }

// // Log the first few samples (for debugging purposes)
// const int32 NumSamplesToLog = FMath::Min(10, Data.Num());
// for (int32 i = 0; i < NumSamplesToLog; i++)
// {
//   UE_LOG(LogTemp, Display, TEXT("Sample %d: %f"), i, Data[i]);
// }

// // Calculate the size of the binary data
// int32 DataSize = Data.Num() * sizeof(float);

// // Allocate an array of bytes with the size of the binary data
// TArray<uint8> BinaryData;
// BinaryData.AddUninitialized(DataSize);

// // Copy the float array memory into the byte array
// FMemory::Memcpy(BinaryData.GetData(), Data.GetData(), DataSize);

// WhisperWebsocket->Send(&BinaryData, DataSize, true);
// }

// void AAvatarsPlayerController::OnAudioBytesExported(bool bSucceeded, const TArray64<uint8>& Data)
// {
//   if (DoNTimes < 5)
//   {
//     DoNTimes++;
//     GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::SanitizeFloat((float)Data.Num()));
//     if (Data.Num() > 0)
//     {
//       GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::SanitizeFloat(Data[0]));
//     }
//   }
//   int32 DataSize = Data.Num() * sizeof(uint8);
//   WhisperWebsocket->Send(Data.GetData(), DataSize, true);
// }