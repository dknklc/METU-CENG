import React from 'react';
import { styled, alpha } from '@mui/material/styles';
import Paper from '@mui/material/Paper';
import { ViewState , EditingState,IntegratedEditing } from '@devexpress/dx-react-scheduler';
import {
  Scheduler,
  DayView,
  WeekView,
  MonthView,
  Appointments,
  AppointmentTooltip,
  AppointmentForm,
  ConfirmationDialog,
  DateNavigator,
  TodayButton,
  Toolbar,
  ViewSwitcher,
  EditRecurrenceMenu,
  DragDropProvider,
} from '@devexpress/dx-react-scheduler-material-ui';
//import {appointments} from './demo-data/appointments_for_resources';

const PREFIX = 'App';
const classes = {
  todayCell: `${PREFIX}-todayCell`,
  weekendCell: `${PREFIX}-weekendCell`,
  today: `${PREFIX}-today`,
  weekend: `${PREFIX}-weekend`,
};

/* 1) These are styled components styled(Component) : the component inside styled is the component that will be wrapped. */
const StyledWeekViewTimeTableCell = styled(WeekView.TimeTableCell)(({ theme }) => ({
  [`&.${classes.todayCell}`]: {
    backgroundColor: alpha(theme.palette.primary.main, 0.1),
    '&:hover': {
      backgroundColor: alpha(theme.palette.primary.main, 0.14),
    },
    '&:focus': {
      backgroundColor: alpha(theme.palette.primary.main, 0.16),
    },
  },
  [`&.${classes.weekendCell}`]: {
    backgroundColor: alpha(theme.palette.action.disabledBackground, 0.04),
    '&:hover': {
      backgroundColor: alpha(theme.palette.action.disabledBackground, 0.04),
    },
    '&:focus': {
      backgroundColor: alpha(theme.palette.action.disabledBackground, 0.04),
    },
  },
}));

const StyledWeekViewDayScaleCell = styled(WeekView.DayScaleCell)(({ theme }) => ({
  [`&.${classes.today}`]: {
    backgroundColor: alpha(theme.palette.primary.main, 0.16),
  },
  [`&.${classes.weekend}`]: {
    backgroundColor: alpha(theme.palette.action.disabledBackground, 0.06),
  },
}));

const TimeTableCell = (props) => {
  const { startDate } = props;
  const date = new Date(startDate);

  if (date.getDate() === new Date().getDate()) {
    return <StyledWeekViewTimeTableCell {...props} className={classes.todayCell} />;
  } if (date.getDay() === 0 || date.getDay() === 6) {
    return <StyledWeekViewTimeTableCell {...props} className={classes.weekendCell} />;
  } return <StyledWeekViewTimeTableCell {...props} />;
};

const DayScaleCell = (props) => {
  const { startDate, today } = props;

  if (today) {
    return <StyledWeekViewDayScaleCell {...props} className={classes.today} />;
  } if (startDate.getDay() === 0 || startDate.getDay() === 6) {
    return <StyledWeekViewDayScaleCell {...props} className={classes.weekend} />;
  } return <StyledWeekViewDayScaleCell {...props} />;
};


const dragDisableIds = new Set([3, 8, 10, 12]);
const allowDrag = ({id}) => !dragDisableIds.has(id);

const appointmentComponent = (props) => {
  if (allowDrag(props.data)) {
    return <Appointments.Appointment {...props} />;
  } return <Appointments.Appointment {...props} style={{ ...props.style, cursor: 'not-allowed' }} />;
};


class Calendar extends React.PureComponent{
  constructor(props){
    super(props);
    this.state = {
      data : this.props.data,
      currentViewName : "Week",

      addedAppointment : {},
      appointmentChanges : {},
      editingAppointment : undefined,
    };
    this.currentViewNameChange = (currentViewName) => {
      this.setState({
        currentViewName : currentViewName
      });
    }
    this.commitChanges = this.commitChanges.bind(this);
    this.changeAddedAppointment = this.changeAddedAppointment.bind(this);
    this.changeAppointmentChanges = this.changeAppointmentChanges.bind(this);
    this.changeEditingAppointment = this.changeEditingAppointment.bind(this);
  }

  changeAddedAppointment(addedAppointment){
    this.setState({addedAppointment});
  }
  changeAppointmentChanges(appointmentChanges){
    this.setState({appointmentChanges});
  }
  changeEditingAppointment(editingAppointment){
    this.setState({editingAppointment});
  }

  commitChanges({added , changed , deleted}){
    this.setState((state) => {
      let {data} = state;
      if(added){
        const startingAddedId = data.length > 0 ? data[data.length -1].id + 1 : 0;
        data = [...data, { id : startingAddedId , ...added}];
      }
      if(changed){
        data = data.map(appointment => (
          changed[appointment.id] ? {...appointment, ...changed[appointment.id]} : appointment
        ));
      }
      if (deleted !== undefined) {
        data = data.filter(appointment => appointment.id !== deleted);
      }
      return {data};
    });
  }
  componentDidUpdate(prevProps) {
    if(prevProps.data.length !== this.props.data.length) {
      this.setState({data: this.props.data});
    }
  }


  render(){
    const { data , currentViewName , addedAppointment , appointmentChanges , editingAppointment} = this.state;
    return (
      
      <Paper>
        <Scheduler data={data} height={660}>        {/* The Scheduler displays data specified via the data property.  Data objects should have the same structure as the AppointmentModel. */}
          <ViewState defaultCurrentDate={new Date()} defaultCurrentViewName={currentViewName} onCurrentViewNameChange={this.currentViewNameChange} />   {/* To specify the date that should be initially displayed in the date navigator and used for calculations, use the ViewState plugin's currentDate option.*/} {/* manages the current view's state */}
          <WeekView startDayHour={2} endDayHour={24} timeTableCellComponent={TimeTableCell} dayScaleCellComponent={DayScaleCell} />
          <WeekView
            name="work-week"      /* our new view option */
            displayName="Work Week"
            excludedDays={[0, 6]}
            startDayHour={9}
            endDayHour={19}
          />
          <DayView startDayHour={9} endDayHour={19} />  {/* intervalCount={2} : gözüken gün sayısını belirler*/} 
          <MonthView />

          <EditingState
            onCommitChanges={this.commitChanges} 
            addedAppointment={addedAppointment}
            onAddedAppointmentChange={this.changeAddedAppointment}
            appointmentChanges={appointmentChanges}
            onAppointmentChangesChange={this.changeAppointmentChanges}
            editingAppointment={editingAppointment}
            onEditingAppointmentChange={this.changeEditingAppointment}
          />
          <IntegratedEditing />           {/* implements editing */}
          <EditRecurrenceMenu />          { /* renders the edit menu and allows you to process user edits */ }
          <ConfirmationDialog />

          <Toolbar />
          <DateNavigator />          {/* renders a date navigator control */}
          <TodayButton />            {/* a button that is used to navigate to the today's date */}
          <ViewSwitcher />           {/* renders the view switcher */}
          <Appointments appointmentComponent={appointmentComponent}/>
          <AppointmentTooltip        
            showOpenButton
            showDeleteButton
            showCloseButton
          />                          {/* An appointment tooltip displays information about the appointment and contains buttons that manage the appointment. */}
          <AppointmentForm/>          {/* <AppointmentForm readonly /> : just allowed reading */} {/*  renders a form that allows a user to edit an appointment */}

          <DragDropProvider
            allowDrag={allowDrag}
          />
        </Scheduler>
      </Paper>
    );
  }
  
}

export default Calendar;
