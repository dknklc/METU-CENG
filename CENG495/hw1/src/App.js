import React , { Component } from 'react';
import { motion } from 'framer-motion';
import images from './images';
import "./App.css";
import Button from '@mui/material/Button';
import axios from 'axios';
import Calendar from './Calendar';

class App extends Component{
  state ={
    events : [],
    width : 0,
    secondevents : [],
    thirdevents : [],
    data : []
  }

  constructor(props) {
    super(props);
    this.carousel = React.createRef();
    this.handleClick = this.handleClick.bind(this);
  }
  componentDidMount(){
    axios.get('https://api.seatgeek.com/2/events',{
      params:{
        client_id:'MjY0ODE0Mzl8MTY0OTQ5Njc0OC4xOTg0Nzk0'
      }
    })
    .then(response => response.data)
    .then(response => {
      this.setState({
        events : response.events,
        width : 2.4*this.carousel.current.scrollWidth - this.carousel.current.offsetWidth,
      })
      //console.log(this.state.events);
    });

    axios.get('https://app.ticketmaster.com/discovery/v2/events.json',{
      params:{
        apikey:'t1N8Hi7sCfnnhN746YL47BiX0pI1Oofb',
        size:10
      }
    })
    .then(response => response.data)
    .then(response => {
      this.setState({
        secondevents : response._embedded.events,
        width : 2.4*this.carousel.current.scrollWidth - this.carousel.current.offsetWidth,
      })
      //console.log(this.state.secondevents);
    });

    axios.get('https://api.currentsapi.services/v1/latest-news',{
      params:{
        apiKey:'3BGR6NeFOTsEgAKpzjVEOHTyu1q_nRTrTci-f6KgTV5lFJtr',
        language:'en'
      }
    })
    .then(response => response.data)
    .then(response => {
      this.setState({
        thirdevents : response.news,
        width : 2.4*this.carousel.current.scrollWidth - this.carousel.current.offsetWidth,
      })
      //console.log(this.state.thirdevents);
    });                              
  }
  handleClick(e){
      this.setState({
        data :  [...this.state.data , {id : 3 , startDate : new Date(e)}]
      });
  }
    render(){
      return (
        <div className="App">
          <motion.div ref={this.carousel} className='carousel' whileTap={{cursor : "grabbing"}}>
            <h1 className='baslik'>Events in US and Canada</h1>
            <motion.div drag="x" dragConstraints={{right:0, left: - this.state.width}} className='inner-carousel'>
              {this.state.events.map((event) => {
                return(
                  <motion.div className="item" key={event.id}>
                    <p className='parag'>{event.short_title}</p>
                    <img src={event.performers[0].image} alt="" />
                    <Button className="button" variant="outlined" onClick={() => this.handleClick(new Date(event.datetime_utc).toISOString().substring(0,10))}>{new Date(event.datetime_utc).toISOString().substring(0,10)}</Button>
                  </motion.div>
                );
              })};
            </motion.div>
          </motion.div>   
          <motion.div ref={this.carousel} className='carousel' whileTap={{cursor : "grabbing"}}>
            <h1 className='baslik'>International Events</h1>
            <motion.div drag="x" dragConstraints={{right:0, left: - this.state.width}} className='inner-carousel'>
              {this.state.secondevents.map((event) => {
                return(
                  <motion.div className="item" key={event.id}>
                    <p className='parag'>{event.name}</p>
                    <img src={event.images[0].url} alt="" />
                    <Button className="button" variant="outlined" onClick={() => this.handleClick(new Date(event.dates.start.localDate).toISOString().substring(0,10))}>{event.dates.start.localDate}</Button>
                  </motion.div>
                );
              })};
            </motion.div>
          </motion.div>  
          <motion.div ref={this.carousel} className='carousel' whileTap={{cursor : "grabbing"}}>
            <h1 className='baslik'>News</h1>
            <motion.div drag="x" dragConstraints={{right:0, left: - this.state.width}} className='inner-carousel'>
              {this.state.thirdevents.slice(0,10).map((event) => {
                return(
                  <motion.div className="item" key={event.id}>
                    <p className='parag'>{event.title.slice(0,40)}</p>
                    {event.image.length > 10 ? <img src={event.image} alt="" /> : <img src={images[0]} alt="" />}
                    <Button className="button" variant="outlined" onClick={() => this.handleClick(new Date(event.published).toISOString().substring(0,10))}>{new Date(event.published).toISOString().substring(0,10)}</Button>
                  </motion.div>
                );
              })};
            </motion.div>
          </motion.div>
          <h2 className='scheduler-baslik'>Scheduler</h2>   
          <Calendar data={this.state.data}/>
        </div>
      );
    }
  }

export default App;
