import React from 'react';
import SwiperCore, { Autoplay } from 'swiper';
import { Swiper, SwiperSlide } from 'swiper/react';
import './heroslide.scss';
import {HeroSlideData} from '../../data/HeroSlideData';

const HeroSlide = () => {
    SwiperCore.use([Autoplay]);
    const HeroSlideDatas = HeroSlideData;

    return (
        <div className="hero-slide">
            <Swiper
                modules={[Autoplay]}
                grabCursor={true}
                spaceBetween={0}
                slidesPerView={1}
                //autoplay={{delay: 3000}}
            >
                {
                    HeroSlideDatas.map((item, i) => (
                        <SwiperSlide key={item.id}>
                            {({ isActive }) => (
                                <img src={item.img} alt='' />
                            )}
                        </SwiperSlide>
                    ))
                }
            </Swiper>
           
        </div>
    );
}

export default HeroSlide;