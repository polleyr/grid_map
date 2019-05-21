/*
 * grid_map_iterator_benchmark.hpp
 *
 *  Created on: Feb 15, 2016
 *     Authors: Christos Zalidis, Péter Fankhauser
 */

#include <grid_map_core/grid_map_core.hpp>
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;
using namespace grid_map;

#define duration(a) duration_cast<milliseconds>(a).count()
typedef high_resolution_clock clk;

/*!
 * Convenient use of iterator.
 */
void runGridMapIteratorVersion1(GridMap& map, const string& layer_from, const string& layer_to)
{
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const float value_from = map.at(layer_from, *iterator);
    float& value_to = map.at(layer_to, *iterator);
    //value_to = value_to > value_from ? value_to : value_from;
    if (value_to <= value_from){
        map.at(layer_to, *iterator)=value_from;
    }
    
  }
}

void streetrunGridMapIteratorVersion1(GridMap& map, GridMap& streetMap, const string& layer_from, const string& layer_to)
{
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
      Position position;
      map.getPosition(*iterator, position);
      if(streetMap.isInside(position)){
        map.at(layer_to, *iterator) = streetMap.atPosition(layer_from, position);
      } else {
        cout<<"error"<<endl;
      }
  }
}


/*!
 * Improved efficiency by storing direct access to data layers.
 */
void streetrunGridMapIteratorVersion2(GridMap& map, GridMap& streetMap, const string& layer_from, const string& layer_to)
{
  const auto& data_from = streetMap[layer_from];
  auto& data_to = map[layer_to];
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const Index index(*iterator);
    Position position;
    map.getPosition(*iterator, position);
     if(streetMap.isInside(position)){
        Index streetIndex;
        streetMap.getIndex(position, streetIndex);
        const float value_from = data_from(streetIndex(0), streetIndex(1));
        float& value_to = data_to(index(0), index(1));
        value_to = value_from;
      } else {
        cout<<"error"<<endl;
      }
  }
}

/*!
 * Improved efficiency by storing direct access to data layers.
 */
void streetrunGridMapIteratorVersion3(GridMap& map, GridMap& streetMap, const string& layer_from, const string& layer_to)
{
  const auto& data_from = streetMap[layer_from];
  auto& data_to = map[layer_to];
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const size_t index = iterator.getLinearIndex();
    Position position;
    map.getPosition(*iterator, position);
     if(streetMap.isInside(position)){
        Index streetIndex;
        streetMap.getIndex(position, streetIndex);
        const float value_from = data_from(streetIndex(0), streetIndex(1));
        float& value_to = data_to(index);
        value_to = value_from;
      } else {
        cout<<"error"<<endl;
      }
  }
}

/*!
 * Improved efficiency by storing direct access to data layers.
 */
void streetrunGridMapIteratorVersion4(GridMap& map, GridMap& streetMap, const string& layer_from, const string& layer_to)
{
  const auto& data_from = streetMap[layer_from];
  auto& data_to = map[layer_to];
  Index zeroIndex(0, 0);
  Position mapzeroposition;
  map.getPosition(zeroIndex, mapzeroposition);
  cout<<"mapzero position " << mapzeroposition(0) <<" "<< mapzeroposition(1)<<endl;
  
  
  Index tfZeroIndex;
  streetMap.getIndex(mapzeroposition, tfZeroIndex);
  
  cout<<tfZeroIndex(0) << " " << tfZeroIndex(1) << endl;

  Index oneIndex(1, 1);
  Position maponeposition;
  map.getPosition(oneIndex, maponeposition);
  cout<<"maponeposition position " << maponeposition(0) <<" "<< maponeposition(1)<<endl;
  
  Position maptoprightposition;
  Index toprightindex(9, 9);
  map.getPosition(toprightindex, maptoprightposition);
  cout<<"maptopright position " << maptoprightposition(0) <<" "<< maptoprightposition(1)<<endl;

  Index tfoneIndex(oneIndex(0)+tfZeroIndex(0), oneIndex(1)+tfZeroIndex(1));
  Position tfmaponeposition;
  streetMap.getPosition(tfoneIndex, tfmaponeposition);
  cout<<"tf streetmaponeposition position " << tfmaponeposition(0) <<" "<< tfmaponeposition(1)<<endl;
  
  if (!(streetMap.isInside(mapzeroposition) and streetMap.isInside(maptoprightposition)))
  {
    cout<<"error"<<endl;
  } else {
  
  
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const Index index(*iterator);
    Index streetIndex(index(0) + tfZeroIndex(0), index(1) + tfZeroIndex(1));
    const float value_from = data_from(streetIndex(0), streetIndex(1));
    float& value_to = data_to(index(0), index(1));
    value_to = value_from;
  }
  }
}



/*!
 * Improved efficiency by storing direct access to data layers.
 */
void runGridMapIteratorVersion2(GridMap& map, const string& layer_from, const string& layer_to)
{
  const auto& data_from = map[layer_from];
  auto& data_to = map[layer_to];
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const Index index(*iterator);
    const float value_from = data_from(index(0), index(1));
    float& value_to = data_to(index(0), index(1));
    value_to = value_to > value_from ? value_to : value_from;
  }
}

/*!
 * Improved efficiency by using linear index.
 */
void runGridMapIteratorVersion3(GridMap& map, const string& layer_from, const string& layer_to)
{
  const auto& data_from = map[layer_from];
  auto& data_to = map[layer_to];
  for (GridMapIterator iterator(map); !iterator.isPastEnd(); ++iterator) {
    const size_t i = iterator.getLinearIndex();
    const float value_from = data_from(i);
    float& value_to = data_to(i);
    value_to = value_to > value_from ? value_to : value_from;
  }
}

/*!
 * Whenever possible, make use of the Eigen methods for maximum efficiency
 * and readability.
 */
void runEigenFunction(GridMap& map, const string& layer_from, const string& layer_to)
{
  map[layer_to] = map[layer_to].cwiseMax(map[layer_from]);
}

/*!
 * For comparison.
 */
void runCustomIndexIteration(GridMap& map, const string& layer_from, const string& layer_to)
{
  const auto& data_from = map[layer_from];
  auto& data_to = map[layer_to];
  for (size_t j = 0; j < data_to.cols(); ++j) {
    for (size_t i = 0; i < data_to.rows(); ++i) {
      const float value_from = data_from(i, j);
      float& value_to = data_to(i, j);
      value_to = value_to > value_from ? value_to : value_from;
    }
  }
}

/*!
 * For comparison.
 */
void runCustomLinearIndexIteration(GridMap& map, const string& layer_from, const string& layer_to)
{
  const auto& data_from = map[layer_from];
  auto& data_to = map[layer_to];
  for (size_t i = 0; i < data_to.size(); ++i) {
    data_to(i) = data_to(i) > data_from(i) ? data_to(i) : data_from(i);
  }
}

int main(int argc, char* argv[])
{
  GridMap map;
  map.setGeometry(Length(51.2, 51.2), 0.01, Position(0.0, 20.0));
  map.add("random");
  map["random"].setRandom();
  map.add("layer1", 0.0);
  map.add("layer2", 0.0);
  map.add("layer3", 0.0);
  map.add("layer4", 0.0);
  map.add("layer5", 0.0);
  map.add("layer6", 0.0);

  GridMap streetMap;
  streetMap.setGeometry(Length(1000.0, 1000.0), 0.1, Position(4.0, 4.0));
  streetMap.add("random");
  streetMap["random"].setRandom();
  streetMap.add("layer1", 0.0);
  streetMap.add("layer2", 0.0);
  streetMap.add("layer3", 0.0);
  streetMap.add("layer4", 0.0);
  streetMap.add("layer5", 0.0);
  streetMap.add("layer6", 0.0);

  
  cout << "Results for iteration over " << map.getSize()(0) << " x " << map.getSize()(1) << " (" << map.getSize().prod() << ") grid cells." << endl;
  cout << "=========================================" << endl;

  clk::time_point t1 = clk::now();
  //streetrunGridMapIteratorVersion1(map, streetMap, "random", "layer1");
  clk::time_point t2 = clk::now();
  cout << "Duration grid map iterator (convenient use): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  //streetrunGridMapIteratorVersion2(map, streetMap, "random", "layer2");
  t2 = clk::now();
  cout << "Duration grid map iterator (direct access to data layers): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  //streetrunGridMapIteratorVersion3(map, streetMap, "random", "layer3");
  t2 = clk::now();
  cout << "Duration grid map iterator (linear index): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  streetrunGridMapIteratorVersion4(map, streetMap, "random", "layer4");
  t2 = clk::now();
  cout << "Duration grid map iterator (direct access to data layers tfed index): " << duration(t2 - t1) << " ms" << endl;
  
  
  
  t1 = clk::now();
  runGridMapIteratorVersion1(map, "random", "layer1");
  t2 = clk::now();
  cout << "Duration grid map iterator (convenient use): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  runGridMapIteratorVersion2(map, "random", "layer2");
  t2 = clk::now();
  cout << "Duration grid map iterator (direct access to data layers): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  runGridMapIteratorVersion3(map, "random", "layer3");
  t2 = clk::now();
  cout << "Duration grid map iterator (linear index): " << duration(t2 - t1) << " ms" << endl;

  t1 = clk::now();
  runEigenFunction(map, "random", "layer4");
  t2 = clk::now();
  cout << "Duration Eigen function: " << duration(t2 - t1) << " ms" <<  endl;

  t1 = clk::now();
  runCustomIndexIteration(map, "random", "layer5");
  t2 = clk::now();
  cout << "Duration custom index iteration: " << duration(t2 - t1) << " ms" <<  endl;

  t1 = clk::now();
  runCustomLinearIndexIteration(map, "random", "layer6");
  t2 = clk::now();
  cout << "Duration custom linear index iteration: " << duration(t2 - t1) << " ms" <<  endl;

  return 0;
}
