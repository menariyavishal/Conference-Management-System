import React, { useRef } from 'react';
import { motion } from 'framer-motion';
import { ChevronLeft, ChevronRight, Calendar, Clock } from 'lucide-react';

const conferences = [
  { name: "IEEE Global AI Summit", date: "May 15-17, 2026", deadline: "Sub deadline: Apr 30", img: "https://images.unsplash.com/photo-1540575467063-178a50c2df87?w=500&auto=format" },
  { name: "International Research Conf", date: "June 10-12, 2026", deadline: "Sub deadline: May 20", img: "https://images.unsplash.com/photo-1505373877841-8d25f7d46678?w=500&auto=format" },
  { name: "Future of Computing", date: "July 5-7, 2026", deadline: "Sub deadline: June 15", img: "https://images.unsplash.com/photo-1515187029135-18ee286d815b?w=500&auto=format" },
  { name: "Sustainable Tech Forum", date: "Aug 20-22, 2026", deadline: "Sub deadline: July 30", img: "https://images.unsplash.com/photo-1475721027785-f74eccf877e2?w=500&auto=format" },
  { name: "Digital Health Symposium", date: "Sep 12-14, 2026", deadline: "Sub deadline: Aug 25", img: "https://images.unsplash.com/photo-1576091160399-112ba8d25d1d?w=500&auto=format" }
];

const ConferenceSlider: React.FC = () => {
  const scrollRef = useRef<HTMLDivElement>(null);

  const scroll = (direction: 'left' | 'right') => {
    if (scrollRef.current) {
      const { scrollLeft, clientWidth } = scrollRef.current;
      const scrollTo = direction === 'left' ? scrollLeft - clientWidth : scrollLeft + clientWidth;
      scrollRef.current.scrollTo({ left: scrollTo, behavior: 'smooth' });
    }
  };

  return (
    <section className="relative z-10 py-16">
      <div className="max-w-7xl mx-auto px-5 sm:px-8 mb-8 flex justify-between items-end">
        <div>
          <h2 className="text-3xl md:text-4xl font-bold mb-2">Featured Conferences</h2>
          <p className="text-slate-400">Join the world's leading academic gatherings.</p>
        </div>
        <div className="flex gap-3">
          <button 
            onClick={() => scroll('left')}
            className="p-3 rounded-full bg-white/5 border border-white/10 hover:bg-white/10 transition"
          >
            <ChevronLeft size={20} />
          </button>
          <button 
            onClick={() => scroll('right')}
            className="p-3 rounded-full bg-white/5 border border-white/10 hover:bg-white/10 transition"
          >
            <ChevronRight size={20} />
          </button>
        </div>
      </div>

      <div 
        ref={scrollRef}
        className="flex gap-6 overflow-x-auto px-5 sm:px-8 pb-8 no-scrollbar scroll-smooth"
      >
        {conferences.map((conf, i) => (
          <motion.div
            key={i}
            whileHover={{ y: -8, scale: 1.02 }}
            className="flex-shrink-0 w-80 bg-white/5 backdrop-blur-md rounded-2xl border border-white/10 overflow-hidden shadow-2xl transition-all duration-300"
          >
            <div className="relative h-44 overflow-hidden">
              <img src={conf.img} alt={conf.name} className="w-full h-full object-cover" />
              <div className="absolute inset-0 bg-gradient-to-t from-[#0f172a] to-transparent opacity-60" />
            </div>
            <div className="p-6">
              <h3 className="text-xl font-bold mb-3">{conf.name}</h3>
              <div className="space-y-2">
                <div className="flex items-center gap-2 text-slate-300 text-sm">
                  <Calendar size={14} className="text-blue-400" />
                  {conf.date}
                </div>
                <div className="flex items-center gap-2 text-amber-400 text-sm">
                  <Clock size={14} />
                  {conf.deadline}
                </div>
              </div>
              <button className="mt-6 w-full py-3 rounded-xl bg-white/5 border border-white/10 hover:bg-blue-600 hover:border-blue-600 transition-all duration-300 text-sm font-semibold">
                View Details
              </button>
            </div>
          </motion.div>
        ))}
      </div>
    </section>
  );
};

export default ConferenceSlider;
