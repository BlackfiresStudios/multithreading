		void threadFindDuplicateIndex(std::vector<IndexInfo>& destination, std::vector<IndexInfo>& fromFile, IndexRange range , std::mutex& destinationBlock)
		{
			bool findFlag = false;


			uint32_t destSize = 0;

			while (true)
			{
				if (cout.try_lock())
				{
					std::cout << range.startID << " " << range.endID << std::endl;
					cout.unlock();
					break;
				}
			}
			
			for (uint32_t ID = range.startID; ID < range.endID; ID++)
			{
				findFlag = false;
				destSize = destination.size();
				
				
				if (destSize > 0)
				{
					for (uint32_t i = 0; i < destSize; i++)
					{
						if (destination[i].positionVertexIndex == fromFile[ID].positionVertexIndex)
						{
							if (destination[i].normalVectorIndex == fromFile[ID].normalVectorIndex)
							{
								if (destination[i].textureCoordsVectorIndex == fromFile[ID].textureCoordsVectorIndex)
								{
									findFlag = true;
									break;
								}
							}
						}
					}
				}		
				if (!findFlag)
				{
					while (true)
					{
						if (destinationBlock.try_lock())
						{
							destination.push_back(fromFile[ID]);
							destinationBlock.unlock();
							break;
						}
					}
				}
			}			
		}
		void searchDuplicatesMultiTask(std::vector<IndexInfo>& destination, std::vector<IndexInfo>& fromFile)
		{
			std::mutex destinationBlock;
			
			const uint32_t dataSize = fromFile.size();

			std::thread** workers = new std::thread * [*multiThreadMode];
			IndexRange* ranges = new IndexRange[*multiThreadMode];

			const uint32_t perThreadData = dataSize / *multiThreadMode;

			IndexRange rangeCache;


			for (uint32_t i = 0, start = 0, end = perThreadData; i < *multiThreadMode; i++)
			{
				if (i == *multiThreadMode - 1)
				{
					end = fromFile.size();
				}

				rangeCache.startID = start;
				rangeCache.endID = end;

				workers[i] = new std::thread(&OBJ_Loader::Loader::threadFindDuplicateIndex, this, std::ref(destination), std::ref(fromFile), rangeCache, std::ref(destinationBlock));
				start += perThreadData;
				end += perThreadData;
			}

			destination.resize(1);

			for (uint32_t i = 0; i < *multiThreadMode; i++)
			{
				workers[i]->join();
			}
		}