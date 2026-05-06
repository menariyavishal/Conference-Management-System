import React from 'react';
import { motion } from 'framer-motion';
import { ArrowRight, Shield, Zap, Users } from 'lucide-react';
import { Link } from 'react-router-dom';
import { useAuth } from '../../context/AuthContext';

const Hero: React.FC = () => {
  const { isAuthenticated } = useAuth();

  const headingText = "Elevating Academic Collaboration";

  return (
    <section className="relative z-10 pt-32 md:pt-44 pb-20 overflow-hidden">
      {/* Static Animated Orbs (Removed mouse following) */}
      <div className="fixed inset-0 overflow-hidden pointer-events-none z-0">
        <motion.div 
          animate={{ 
            scale: [1, 1.1, 1],
            opacity: [0.2, 0.3, 0.2] 
          }}
          transition={{ duration: 8, repeat: Infinity, ease: "easeInOut" }}
          className="absolute top-20 left-10 w-96 h-96 bg-blue-600/30 rounded-full blur-[100px]" 
        />
        <motion.div 
          animate={{ 
            scale: [1, 1.2, 1],
            opacity: [0.1, 0.2, 0.1] 
          }}
          transition={{ duration: 10, repeat: Infinity, ease: "easeInOut", delay: 1 }}
          className="absolute bottom-20 right-10 w-80 h-80 bg-purple-600/30 rounded-full blur-[100px]" 
        />
      </div>

      <div className="max-w-7xl mx-auto px-5 sm:px-8 text-center relative z-10">
        <motion.h1 
          className="text-5xl sm:text-6xl md:text-7xl font-extrabold leading-tight tracking-tight mb-8"
          initial="hidden"
          animate="visible"
        >
          {headingText.split("").map((char, index) => (
            <motion.span
              key={index}
              variants={{
                hidden: { opacity: 0, y: 20 },
                visible: { opacity: 1, y: 0 }
              }}
              transition={{ delay: index * 0.03, duration: 0.5 }}
              className="inline-block"
            >
              {char === " " ? "\u00A0" : char}
            </motion.span>
          ))}
        </motion.h1>

        <motion.p 
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ delay: 1, duration: 0.8 }}
          className="text-xl text-slate-400 max-w-2xl mx-auto mb-10 leading-relaxed"
        >
          A high‑performance, secure, and intuitive platform built on a 
          <span className="text-blue-400 font-mono mx-2">C++17 core</span> 
          for researchers, reviewers, and organizers worldwide.
        </motion.p>

        <motion.div 
          initial={{ opacity: 0, scale: 0.9 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ delay: 1.2 }}
          className="flex flex-wrap justify-center gap-5 mb-20"
        >
          {!isAuthenticated ? (
            <Link to="/register" className="btn-primary flex items-center gap-2 group text-lg px-8 py-4">
              Submit Your Paper 
              <ArrowRight size={20} className="group-hover:translate-x-1 transition-transform" />
            </Link>
          ) : (
            <Link to="/dashboard" className="btn-primary flex items-center gap-2 group text-lg px-8 py-4">
              Go to Dashboard 
              <ArrowRight size={20} className="group-hover:translate-x-1 transition-transform" />
            </Link>
          )}
          <Link to="/conferences" className="glass px-8 py-4 rounded-xl text-lg font-semibold hover:bg-white/10 transition shadow-md border border-white/10">
            Browse Conferences
          </Link>
        </motion.div>

        {/* Stats Grid */}
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-6 max-w-4xl mx-auto">
          {[
            { icon: <Shield size={24} className="text-blue-400" />, title: "256-bit Encryption", desc: "End-to-end secure submissions" },
            { icon: <Zap size={24} className="text-purple-400" />, title: "Real-time Analytics", desc: "Track submissions & reviews" },
            { icon: <Users size={24} className="text-cyan-400" />, title: "Automated Peer Review", desc: "Double‑blind, AI‑assisted" }
          ].map((item, i) => (
            <motion.div
              key={i}
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              viewport={{ once: true }}
              transition={{ delay: 1.5 + i * 0.1 }}
              className="glass-card !p-6 hover:bg-white/5"
            >
              <div className="mb-3">{item.icon}</div>
              <h3 className="font-bold text-white mb-1">{item.title}</h3>
              <p className="text-sm text-slate-400">{item.desc}</p>
            </motion.div>
          ))}
        </div>
      </div>
    </section>
  );
};

export default Hero;
